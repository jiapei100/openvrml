# $Id: make-php-trans.awk,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $

BEGIN {
  msgid=""; msgstr="";
  print ("<?php\n");
}

/^msgid "/ { #"{
  if (msgid && str) {
    gsub(/\$/, "\\$", str);
    print ("$locale[\"" msgid "\"] =\n   \"" str "\";");
  }
  str = substr ($0, 8, length ($0) - 8);
  msgstr="";
}

/^msgstr "/ { #"{
  msgid=str;
  str = substr ($0, 9, length ($0) - 9);
  next;
}

/^"/ { #"{
  str = (str substr ($0, 2, length ($0) - 2));
  next;
}

END {
  if (msgid && str) {
    gsub(/\$/, "\\$", str);
    print ("$locale[\"" msgid "\"] =\n   \"" str "\";");
  }
  print ("\n?>");
}
