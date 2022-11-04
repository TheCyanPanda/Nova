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
import collections
import functools
import time


def generate_str_table(headers: list, attributes: list, space_pad: int = 4) -> str:
    """
    Generate table like:
    header0    header1   header2 ....
    attr01     attr02    attr03  ....
    attr10     attr11    attr12  ....
    """
    lh = len(headers)
    if not lh <= len(attributes):
        raise ValueError("ERROR: length of attributes must be larger or equal to length of headers")

    spaces = [space_pad for _ in headers]
    # Compute spacing between each attribute
    for i, attr in enumerate(attributes):
        val = str(attr).strip()
        space_head = len(str(headers[i % lh]).strip())
        space = (space_head + space_pad) if space_head >= len(val) else (len(val) + space_pad)
        if space > int(spaces[i % lh]):
            spaces[i % lh] = space

    # Fill string with headers
    s = "".join(map(lambda x: f"{headers[x] : <{spaces[x]}}", range(lh)))
    # Fill string with attributes. Create new line for each i == len(headers)
    for i, attr in enumerate(attributes):
        if (i % lh) == 0:
            s += '\n'
        s += f"{str(attr).strip() : <{spaces[i % lh]}}"
    return s


class CommandLineParserException(Exception):
    pass


class CmdWrap:
    argv: list
    argc: int
    doc: str

    def __call__(self, func):
        """
        Pass input string argument to wrapper and parse as
        space separated list. Also collect argv,argc and docstring
        and pass to function
        """
        @functools.wraps(func)
        def wrapper(parent_class, arg: str):
            self.argv = [a.strip() for a in arg.split(' ')]
            self.argc = len(self.argv)
            self.doc = func.__doc__

            return func(parent_class, self)
        return wrapper


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
    help_margin_limits = (84, 84)  # Syntax, Description

    def __init__(self):
        self.__generate_cmd_help()
        cmd.Cmd.__init__(self)
        threading.Thread.__init__(self)
        self.webserver = flaskserver.WebServer()
        self.tcpclient = tcp_client.TcpClient(host='localhost', port=1234)
        self.history = collections.deque()

    def _append_history(self, input_):
        @CmdWrap()
        def _get_args(_, input_line) -> CmdWrap:
            return input_line

        if isinstance(input_, str):
            args = _get_args(None, input_)
        elif isinstance(input_, CmdWrap):
            args = input_
        else:
            s = f"ERROR: Invalid type: {type(input_)}"
            raise TypeError(s)
        self.history.append(args.argv)

    def preloop(self) -> None:
        print(self.banner)
        print(self.cmd_avail)

    @classmethod
    def _get_syntax_and_desc(cls, docstring):
        elm = ['syntax', 'description']  # Search for syntax: ...\n description: ....
        syntax_and_desc = []  # [syntax, description] (type=string)
        for d in docstring.splitlines():
            d = d.strip()
            if len(d) == 0:
                continue
            key, text = d.split(':')
            if key.lower().strip() in elm and text:
                idx = 1 if (key in elm[0]) else 0
                elm.pop(idx)
                syntax_and_desc.append(text)
        return syntax_and_desc

    def __generate_cmd_help(self):
        sxd = ["Available commands:\n", ""]
        do_functions_lst = []
        exceptions = ['do_exit', 'do_help', 'do_q', 'do_quit']
        do_fns = dir(self)
        for d in do_fns:
            if not d.startswith('do_') or d.startswith('do__') or d in exceptions:
                continue
            do_functions_lst.append(d)

        # Iterate over all do_X functions
        for fn_name in do_functions_lst:
            fn = getattr(self, fn_name)
            docstring = fn.__doc__
            syntax_and_desc = self._get_syntax_and_desc(docstring)  # [syntax, description]
            if not len(syntax_and_desc) == 2:
                raise AttributeError("Each do_X function must have syntax and description in the"
                                     " doc-string")
            syntax = syntax_and_desc[0].strip()
            desc = syntax_and_desc[1].strip()
            if len(syntax) > self.help_margin_limits[0] or len(desc) > self.help_margin_limits[1]:
                raise CommandLineParserException(f"Syntax or description to function {fn_name} too long."
                                      f"Syntax/Descr. limits={self.help_margin_limits}")
            sxd.extend([syntax, desc])
        sxd.extend(["q | quit", "Exit this shell"])
        self.cmd_avail += generate_str_table(['', ''], sxd)
        self.cmd_avail += f"\n{'Begin any command with : to run it outside of this shell'}\n"

    def run(self):
        self.cmdloop()

    def precmd(self, line):
        if re.match(f'^:', line):
            try:
                print(f"To be implemented. This was your cmd: {line}")  # todo <--
            except subprocess.CalledProcessError:
                pass
        return line

    def postcmd(self, stop: bool, line: str) -> bool:
        self._append_history(line)
        return stop

    def onecmd(self, line):
        if isinstance(line, str) and len(line) > 0:
            return super().onecmd(line)
        else:
            super().onecmd('_none')
            return

    def postloop(self) -> None:
        print('Goodbye!')

    """ do functions """
    @classmethod
    def do__none(cls, _):
        pass

    @classmethod
    def do_quit(cls, _):
        # todo: close daemon, udp streams etc gracefully before exit
        # todo: close any resources before exit (import atexit ?)
        return True

    def do_q(self, line):
        return self.do_quit(line)

    @CmdWrap()
    def do_history(self, args: CmdWrap):
        """
        Syntax: history [clear]
        Description: Print/clear command history
        """
        if args.argc > 1:
            return self.help_default(args.doc)

        if 'clear' in args.argv[0].lower():
            self.history.clear()
        elif len(args.argv[0]) == 0:
            for c in self.history:
                print(' '.join(x for x in c))
        else:
            return self.help_default(args.doc)

    @CmdWrap()
    def do_daemon(self, args: CmdWrap):
        """
        Syntax: daemon [start <port> | stop | cmd <command>]
        Description: Start/stop NovaDaemon or exec command to it manually
        """
        if not args.argc > 0:
            return self.help_default(args.doc)

        if args.argv[0] == 'start':
            process = subprocess.Popen('../../build/apps/NovaServer')
            # todo: implement tcp_client connect to server and the other stuff
            time.sleep(1)
            self.tcpclient.connect()
        elif args.argv[0] == 'stop':
            # todo: implement this
            pass
        elif args.argv[0] == 'cam_mid':
            resp = self.tcpclient.send_cmd('cam_move MID')
            print(resp)
        elif args.argv[0] == 'cam_max':
            resp = self.tcpclient.send_cmd('cam_move MAX')
            print(resp)
        else:
            return self.help_default(args.doc)

    @CmdWrap()
    def do_web_server(self, args: CmdWrap):
        """
        Syntax: web_server [start [port] | stop]
        Description: Start or stop web server
        """
        if not args.argc > 0:
            return self.help_default(args.doc)

        if args.argv[0] == 'start':
            t1 = threading.Thread(self.webserver.start())
        elif args.argv[0] == 'stop':
            t1 = threading.Thread(self.webserver.shutdown())
        else:
            return self.help_web_server()
        t1.start()

    """ Help functions """

    def help_default(self, docstring):
        """
        Default help function for all do_x methods. Collects the syntax from do_x fn docstring.
        """
        syntax_and_desc = self._get_syntax_and_desc(docstring)
        s = "Invalid syntax."
        try:
            s += " Example usage: \n"
            s += syntax_and_desc[0].strip()
        except IndexError:
            pass
        print(s)

    def help_web_server(self):
        print("Invalid syntax")

    def help_daemon(self):
        print("Invalid syntax")


def main():
    c = CommandLineParser()
    c.start()


if __name__ == '__main__':
    main()
