import unittest
import socket
import time

HOST = '127.0.0.1'
PORT = 8080
SERVER_PASSWORD = 'pass'
TIMEOUT = 3.0

def irc_connect():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(TIMEOUT)
    sock.connect((HOST, PORT))
    return sock

def irc_register(sock, nick, username="user", realname="Real Name"):
    sock.sendall(f"PASS {SERVER_PASSWORD}\r\n".encode())
    time.sleep(0.1)
    sock.sendall(f"NICK {nick}\r\n".encode())
    time.sleep(0.1)
    sock.sendall(f"USER {username} 0 * :{realname}\r\n".encode())
    time.sleep(0.1)

class IrcServerIntegrationTest(unittest.TestCase):

    def setUp(self):
        self.sock = irc_connect()

    def tearDown(self):
        self.sock.close()

    def read_until(self, expected_substring, max_bytes=4096):
        data = b''
        start_time = time.time()
        while time.time() - start_time < TIMEOUT:
            try:
                data += self.sock.recv(max_bytes)
            except socket.timeout:
                break
            if expected_substring in data.decode(errors='ignore'):
                return data.decode(errors='ignore')
        return data.decode(errors='ignore')

    # AUTHENTICATION TESTS
    def test_pass_wrong(self):
        self.sock.sendall(b'PASS wrong\r\n')
        resp = self.read_until('464')
        self.assertIn('464', resp)

    def test_pass_already_registered(self):
        irc_register(self.sock, "passreg")
        self.sock.sendall(f"PASS {SERVER_PASSWORD}\r\n".encode())
        resp = self.read_until('462')
        self.assertIn('462', resp)

    def test_nick_missing(self):
        irc_register(self.sock, "nicktest")
        self.sock.sendall(b'NICK\r\n')
        resp = self.read_until('431')
        self.assertIn('431', resp)

    def test_nick_in_use(self):
        irc_register(self.sock, "nickused")
        sock2 = irc_connect()
        irc_register(sock2, "nickused2")
        sock2.recv(4096)
        sock2.sendall(b'NICK nickused\r\n')
        resp = sock2.recv(4096).decode(errors='ignore')
        self.assertIn('433', resp)
        sock2.close()

    def test_nick_invalid(self):
        irc_register(self.sock, "nickinv")
        self.sock.sendall(b'NICK 123bad\r\n')
        resp = self.read_until('432')
        self.assertIn('432', resp)

    def test_user_missing_params(self):
        self.sock.sendall(f"PASS {SERVER_PASSWORD}\r\n".encode())
        self.sock.sendall(b'NICK usertest\r\n')
        self.sock.sendall(b'USER onlyone\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_user_already_registered(self):
        irc_register(self.sock, "userreg")
        self.sock.sendall(b'USER again 0 * :Again\r\n')
        resp = self.read_until('462')
        self.assertIn('462', resp)

    # PRIVMSG / NOTICE
    def test_privmsg_broadcast(self):
        irc_register(self.sock, "sender")
        self.sock.sendall(b'JOIN #chat\r\n')
        self.read_until('JOIN')

        sock2 = irc_connect()
        irc_register(sock2, "receiver")
        sock2.sendall(b'JOIN #chat\r\n')
        time.sleep(0.2)
        sock2.recv(4096)

        self.sock.sendall(b'PRIVMSG #chat :hello channel\r\n')
        time.sleep(0.2)
        resp = sock2.recv(4096).decode(errors='ignore')
        self.assertIn('hello channel', resp)
        sock2.close()

    def test_privmsg_no_target(self):
        irc_register(self.sock, "privmsg1")
        self.sock.sendall(b'PRIVMSG\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_privmsg_no_such_nick(self):
        irc_register(self.sock, "privmsg2")
        self.sock.sendall(b'PRIVMSG unknown :hello\r\n')
        resp = self.read_until('401')
        self.assertIn('401', resp)

    def test_notice_unknown_nick(self):
        irc_register(self.sock, "notice1")
        self.sock.sendall(b'NOTICE unknown :hello\r\n')
        # Should not error; NOTICE is silent if recipient doesn't exist

    # JOIN / PART
    def test_join_and_duplicate(self):
        irc_register(self.sock, "joindup")
        self.sock.sendall(b'JOIN #dup\r\n')
        self.read_until('JOIN')
        self.sock.sendall(b'JOIN #dup\r\n')
        resp = self.read_until('JOIN')
        self.assertIn('JOIN', resp)

    def test_join_no_channel(self):
        irc_register(self.sock, "joinerr")
        self.sock.sendall(b'JOIN\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_part_no_channel(self):
        irc_register(self.sock, "parterr")
        self.sock.sendall(b'PART\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_part_not_on_channel(self):
        irc_register(self.sock, "partnoton")
        self.sock.sendall(b'PART #nochan\r\n')
        resp = self.read_until('442')
        self.assertIn('442', resp)

    # INVITE
    def test_invite_without_channel(self):
        irc_register(self.sock, "invite1")
        self.sock.sendall(b'INVITE user\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_invite_user_on_channel(self):
        irc_register(self.sock, "op1")
        self.sock.sendall(b'JOIN #invitechan\r\n')
        self.read_until('JOIN')
        sock2 = irc_connect()
        irc_register(sock2, "user2")
        sock2.sendall(b'JOIN #invitechan\r\n')
        self.read_until('JOIN')
        self.sock.sendall(b'INVITE user2 #invitechan\r\n')
        resp = self.read_until('443')  # already in channel
        self.assertIn('443', resp)
        sock2.close()

    # KICK
    def test_kick_not_on_channel(self):
        irc_register(self.sock, "kicknoton")
        self.sock.sendall(b'KICK #nochan user\r\n')
        resp = self.read_until('403')
        self.assertIn('403', resp)

    def test_kick_not_operator(self):
        irc_register(self.sock, "user1")
        self.sock.sendall(b'JOIN #kickchan\r\n')
        self.read_until('JOIN')
        sock2 = irc_connect()
        irc_register(sock2, "user2")
        sock2.sendall(b'JOIN #kickchan\r\n')
        self.read_until('JOIN')
        sock2.sendall(b'KICK #kickchan user1\r\n')
        resp = sock2.recv(4096).decode(errors='ignore')
        self.assertIn('482', resp)
        sock2.close()

    # TOPIC
    def test_topic_no_channel(self):
        irc_register(self.sock, "topicnone")
        self.sock.sendall(b'TOPIC\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_topic_not_on_channel(self):
        irc_register(self.sock, "notopic")
        sock2 = irc_connect()
        irc_register(sock2, "topicmaker")
        sock2.sendall(b'JOIN #topicc\r\n')
        self.read_until('JOIN')
        self.sock.sendall(b'TOPIC #topicc\r\n')
        resp = self.read_until('442')
        self.assertIn('442', resp)
        sock2.close()

    # MODE tests
    def test_mode_no_channel(self):
        irc_register(self.sock, "modeno")
        self.sock.sendall(b'MODE\r\n')
        resp = self.read_until('461')
        self.assertIn('461', resp)

    def test_mode_channel_key_limit_invite(self):
        irc_register(self.sock, "modetest")
        self.sock.sendall(b'JOIN #modetest\r\n')
        self.read_until('JOIN')
        self.sock.sendall(b'MODE #modetest +k secret\r\n')
        self.read_until('MODE')
        self.sock.sendall(b'MODE #modetest +l 1\r\n')
        self.read_until('MODE')
        self.sock.sendall(b'MODE #modetest +i\r\n')
        self.read_until('MODE')

        sock2 = irc_connect()
        irc_register(sock2, "notinvited")
        sock2.sendall(b'JOIN #modetest\r\n')
        resp = sock2.recv(4096).decode(errors='ignore')
        self.assertTrue('473' in resp or '475' in resp or '471' in resp)
        sock2.close()

    def test_mode_topic_restricted(self):
        sock1 = self._connect()
        sock1.sendall(b"NICK op\r\nUSER op 0 * :Operator\r\n")
        sock1.sendall(b"JOIN #topchan\r\n")
        sock1.sendall(b"MODE #topchan +t\r\n")
        sock2 = self._connect()
        sock2.sendall(b"NICK nonop\r\nUSER nonop 0 * :NotOp\r\n")
        sock2.sendall(b"JOIN #topchan\r\n")
        sock2.sendall(b"TOPIC #topchan :new topic\r\n")
        resp = sock2.recv(4096).decode(errors="ignore")
        self.assertIn('482', resp)

    def test_partial_command_delivery(self):
        self.sock.sendall(b'PASS ')
        time.sleep(0.1)
        self.sock.sendall(f'{SERVER_PASSWORD}\r\nNICK'.encode())
        time.sleep(0.1)
        self.sock.sendall(b' testuser\r\n')
        time.sleep(0.1)
        self.sock.sendall(b'USER test 0 * :Real Name\r\n')
        resp = self.read_until('001')
        self.assertIn('001', resp)

    def test_multiple_commands_one_packet(self):
        cmds = (
            f'PASS {SERVER_PASSWORD}\r\n'
            'NICK multi\r\n'
            'USER multi 0 * :Multi Line\r\n'
        )
        self.sock.sendall(cmds.encode())
        resp = self.read_until('001')
        self.assertIn('001', resp)

if __name__ == '__main__':
    unittest.main()
