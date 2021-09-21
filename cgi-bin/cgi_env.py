#!/usr/bin/env python3

import cgi

print('Content-Type: text/html', end='\r\n')

cgi.print_environ()
