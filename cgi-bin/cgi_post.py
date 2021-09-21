#!/usr/bin/env python3

import cgi

form = cgi.FieldStorage()

if form.done == -1 or 'name' not in form or 'lastname' not in form:
    resp = '<p>No form data</p>'
else:
    name = form.getvalue('name')
    lastname = form.getvalue('lastname')

    resp = f'<ul>' \
           f'<li>Name: {name}</li>' \
           f'<li>Lastname: {lastname}</li>' \
           f'</ul>'

print('Content-Type: text/html', end='\r\n')
print(end='\r\n')

print(resp)
