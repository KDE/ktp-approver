#! /usr/bin/env bash
#$EXTRACTRC `find . -name "*.ui"` >> rc.cpp || exit 11
$XGETTEXT `find . -name "*.cpp"` -o $podir/kded_ktp_approver.pot
#rm -f rc.cpp
