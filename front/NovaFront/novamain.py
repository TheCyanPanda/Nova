#!/usr/bin/python3
"""
todo: add docstring here
todo: import logging
"""

import cmd
import re
import flaskserver
import subprocess
import threading
import tcp_client
import time



def get_argv(line: str):
    argv = line.split(' ')
    ret = (len(argv), argv)
    return ret


def shell_cmd(s: str):
    """
    Run command in shell
    """
    return subprocess.run(s,  check=True, capture_output=True)


def nova_cmd(s: str):
    """
    Run command to novaDaemon
    """
    pass


class CommandLineParser(cmd.Cmd, threading.Thread):
    prompt = "NovaServer> "
    banner = " _______                     _______                                \n" \
             "|    |  |.-----.--.--.---.-.|     __|.-----.----.--.--.-----.----. \n" \
             "|       ||  _  |  |  |  _  ||__     ||  -__|   _|  |  |  -__|   _| \n" \
             "|__|____||_____|\\___/|___._||_______||_____|__|  \\___/|_____|__|     "

    cmd_avail = ""

    def __init__(self):
        self._generate_cmd_help()
        cmd.Cmd.__init__(self)
        threading.Thread.__init__(self)
        self.webserver = flaskserver.WebServer()
        self.tcpclient = tcp_client.TcpClient()

    def _generate_cmd_help(self):
        self.cmd_avail = "Available commands: \n"
        do_fn_lst = []
        fn_exceptions = ['do_exit', 'do_help', 'do_quit', 'do_q']

        for d in dir(self):
            if not d.startswith('do_') or d.startswith('do__') or d in fn_exceptions:
                continue
            do_fn_lst.append(d)
        for fn_name in do_fn_lst:
            fn = getattr(self, fn_name)
            docstring = fn.__doc__
            elm = ['syntax', 'description']
            syntax_and_desc = []
            for d in docstring.splitlines():
                d = d.strip()
                if len(d) == 0:
                    continue
                key, text = d.split(':')
                if key.lower().strip() in elm and text:
                    elm.pop(1 if (key in elm[0]) else 0)
                    syntax_and_desc.append(text)
            if not len(syntax_and_desc) == 2:
                raise AttributeError(f"ERROR: Missing attribute in do_x function docstring (requires syntax and "
                                     f"description")
            self.cmd_avail += f"{syntax_and_desc[0].strip() : <85}" \
                              f"{syntax_and_desc[1].lstrip() : ^10}\n"

        self.cmd_avail += f"{'q | quit': <85}{'Exit this shell' : ^10} \n" \
                          f"{'Begin any command with : to run it outside of this shell'} \n"

    def run(self):
        self.cmdloop()

    def preloop(self) -> None:
        print(self.banner)
        print(self.cmd_avail)

    def onecmd(self, line: str) -> bool:
        if isinstance(line, str) and len(line) > 0:
            if not line.startswith(':'):
                return super().onecmd(line)
            else:
                print(f"To be implemented. This was your cmd: {line}")

    def postloop(self) -> None:
        print('Goodbye!')

    """ do functions """

    def do_quit(self, line):
        # todo: close daemon, udp streams etc gracefully before exit
        # todo: close any resources before exit (import atexit ?)
        return True

    def do_q(self, line):
        return self.do_quit(line)

    def do_daemon(self, line: str):
        """
        Syntax: daemon [start <port> | stop] [cmd <command>]
        Description: Start/stop NovaDaemon or exec command to it manually
        """
        argc, argv = get_argv(line)
        if argc > 0:
            if argv[0] == 'start':
                process = subprocess.Popen('../../build/apps/NovaServer')
                # todo: implement tcp_client connect to server and the other stuff
            elif argv[0] == 'stop':
                # todo: implement this
                pass
            elif argv[1] == 'cam_mid':
                resp = self.tcpclient.send_cmd('cam_move MID')
                print(resp)
            else:
                return self.help_daemon


    def do_web_server(self, line: str):
        """
        Syntax: web_server [start [port] | stop]
        Description: Start or stop web server
        """
        argc, argv = get_argv(line)
        print(argc, argv)

        if argc > 0:
            if argv[0] == 'start':
                t1 = threading.Thread(self.webserver.start())
            elif argv[0] == 'stop':
                t1 = threading.Thread(self.webserver.shutdown())
            else:
                return self.help_web_server()
            t1.start()

    """ Help functions """

    def help_web_server(self):
        print("Invalid syntax")

    def help_daemon(self):
        print("Invalid syntax")

def main():
    c = CommandLineParser()
    c.start()


if __name__ == '__main__':
    main()
