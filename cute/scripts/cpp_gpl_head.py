#inserts a GPL head at begining of a file

import string
from cute import *
from time import gmtime, strftime

head = r"""/***************************************************************************
                          $filename  -  $description
                             -------------------
    begin                : $date
    copyright            : (C) $year by $name
    email                : $email
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

"""

path = string.split(fileName(), "/")
filename = path[-1]
description = inputText("Enter short description")
date = strftime("%a, %d %b %Y")
year = strftime("%Y")
name = inputText("Enter your name")
email = inputText("Enter email address")

head = string.replace(head, r"$filename", filename)
head = string.replace(head, r"$description", description)
head = string.replace(head, r"$date", date)
head = string.replace(head, r"$year", year)
head = string.replace(head, r"$name", name)
head = string.replace(head, r"$email", email)

insert(head, 0, 0)

