#!/bin/sh
#
# Copyright (c) 2006 Johannes E. Schindelin
#

test_description='git rerere

! [fifth] version1
 ! [first] first
  ! [fourth] version1
   ! [master] initial
    ! [second] prefer first over second
     ! [third] version2
------
     + [third] version2
+      [fifth] version1
  +    [fourth] version1
+ +  + [third^] third
    -  [second] prefer first over second
 +  +  [first] first
    +  [second^] second
++++++ [master] initial
'

. ./test-lib.sh

test_expect_success 'setup' '
	cat >a1 <<-\EOF &&
	Some title
	==========
	Whether '\''tis nobler in the mind to suffer
	The slings and arrows of outrageous fortune,
	Or to take arms against a sea of troubles,
	And by opposing end them? To die: to sleep;
	No more; and by a sleep to say we end
	The heart-ache and the thousand natural shocks
	That flesh is heir to, '\''tis a consummation
	Devoutly to be wish'\''d.
	EOF

	git add a1 &&
	test_tick &&
	git commit -q -a -m initial &&

	cat >>a1 <<-\EOF &&
	Some title
	==========
	To die, to sleep;
	To sleep: perchance to dream: ay, there'\''s the rub;
	For in that sleep of death what dreams may come
	When we have shuffled off this mortal coil,
	Must give us pause: there'\''s the respect
	That makes calamity of so long life;
	EOF

	git checkout -b first &&
	test_tick &&
	git commit -q -a -m first &&

	git checkout -b second master &&
	git show first:a1 |
	sed -e "s/To die, t/To die! T/" -e "s/Some title/Some Title/" >a1 &&
	echo "* END *" >>a1 &&
	test_tick &&
	git commit -q -a -m second
'

test_expect_success 'nothing recorded without rerere' '
	rm -rf .git/rr-cache &&
	git config rerere.enabled false &&
	test_must_fail git merge first &&
	! test -d .git/rr-cache
'

test_expect_success 'activate rerere, old style (conflicting merge)' '
	git reset --hard &&
	mkdir .git/rr-cache &&
	test_might_fail git config --unset rerere.enabled &&
	test_must_fail git merge first &&

	sha1=$(perl -pe "s/	.*//" .git/MERGE_RR) &&
	rr=.git/rr-cache/$sha1 &&
	grep "^=======\$" $rr/preimage &&
	! test -f $rr/postimage &&
	! test -f $rr/thisimage
'

test_expect_success 'rerere.enabled works, too' '
	rm -rf .git/rr-cache &&
	git config rerere.enabled true &&
	git reset --hard &&
	test_must_fail git merge first &&

	sha1=$(perl -pe "s/	.*//" .git/MERGE_RR) &&
	rr=.git/rr-cache/$sha1 &&
	grep ^=======$ $rr/preimage
'

test_expect_success 'set up rr-cache' '
	rm -rf .git/rr-cache &&
	git config rerere.enabled true &&
	git reset --hard &&
	test_must_fail git merge first &&
	sha1=$(perl -pe "s/	.*//" .git/MERGE_RR) &&
	rr=.git/rr-cache/$sha1
'

test_expect_success 'rr-cache looks sane' '
	# no postimage or thisimage yet
	! test -f $rr/postimage &&
	! test -f $rr/thisimage &&

	# preimage has right number of lines
	cnt=$(sed -ne "/^<<<<<<</,/^>>>>>>>/p" $rr/preimage | wc -l) &&
	echo $cnt &&
	test $cnt = 13
'

test_expect_success 'rerere diff' '
	git show first:a1 >a1 &&
	cat >expect <<-\EOF &&
	--- a/a1
	+++ b/a1
	@@ -1,4 +1,4 @@
	-Some Title
	+Some title
	 ==========
	 Whether '\''tis nobler in the mind to suffer
	 The slings and arrows of outrageous fortune,
	@@ -8,21 +8,11 @@
	 The heart-ache and the thousand natural shocks
	 That flesh is heir to, '\''tis a consummation
	 Devoutly to be wish'\''d.
	-<<<<<<<
	-Some Title
	-==========
	-To die! To sleep;
	-=======
	 Some title
	 ==========
	 To die, to sleep;
	->>>>>>>
	 To sleep: perchance to dream: ay, there'\''s the rub;
	 For in that sleep of death what dreams may come
	 When we have shuffled off this mortal coil,
	 Must give us pause: there'\''s the respect
	 That makes calamity of so long life;
	-<<<<<<<
	-=======
	-* END *
	->>>>>>>
	EOF
	git rerere diff >out &&
	test_cmp expect out
'

test_expect_success 'rerere status' '
	echo a1 >expect &&
	git rerere status >out &&
	test_cmp expect out
'

test_expect_success 'first postimage wins' '
	git show first:a1 | sed "s/To die: t/To die! T/" >expect &&

	git commit -q -a -m "prefer first over second" &&
	test -f $rr/postimage &&

	git checkout -b third master &&
	git show second^:a1 | sed "s/To die: t/To die! T/" >a1 &&
	git commit -q -a -m third &&

	test_must_fail git pull . first &&
	# rerere kicked in
	! grep "^=======\$" a1 &&
	test_cmp expect a1
'

test_expect_success 'rerere clear' '
	rm $rr/postimage &&
	echo "$sha1	a1" | perl -pe "y/\012/\000/" >.git/MERGE_RR &&
	git rerere clear &&
	! test -d $rr
'

test_expect_success 'set up for garbage collection tests' '
	mkdir -p $rr &&
	echo Hello >$rr/preimage &&
	echo World >$rr/postimage &&

	sha2=4000000000000000000000000000000000000000 &&
	rr2=.git/rr-cache/$sha2 &&
	mkdir $rr2 &&
	echo Hello >$rr2/preimage &&

	almost_15_days_ago=$((60-15*86400)) &&
	just_over_15_days_ago=$((-1-15*86400)) &&
	almost_60_days_ago=$((60-60*86400)) &&
	just_over_60_days_ago=$((-1-60*86400)) &&

	test-chmtime =$almost_60_days_ago $rr/preimage &&
	test-chmtime =$almost_15_days_ago $rr2/preimage
'

test_expect_success 'garbage collection preserves young records' '
	git rerere gc &&
	test -f $rr/preimage &&
	test -f $rr2/preimage
'

test_expect_success 'old records rest in peace' '
	test-chmtime =$just_over_60_days_ago $rr/preimage &&
	test-chmtime =$just_over_15_days_ago $rr2/preimage &&
	git rerere gc &&
	! test -f $rr/preimage &&
	! test -f $rr2/preimage
'

test_expect_success 'setup: file2 added differently in two branches' '
	git reset --hard &&

	git checkout -b fourth &&
	echo Hallo >file2 &&
	git add file2 &&
	test_tick &&
	git commit -m version1 &&

	git checkout third &&
	echo Bello >file2 &&
	git add file2 &&
	test_tick &&
	git commit -m version2 &&

	test_must_fail git merge fourth &&
	echo Cello >file2 &&
	git add file2 &&
	git commit -m resolution
'

test_expect_success 'resolution was recorded properly' '
	echo Cello >expected &&

	git reset --hard HEAD~2 &&
	git checkout -b fifth &&

	echo Hallo >file3 &&
	git add file3 &&
	test_tick &&
	git commit -m version1 &&

	git checkout third &&
	echo Bello >file3 &&
	git add file3 &&
	test_tick &&
	git commit -m version2 &&
	git tag version2 &&

	test_must_fail git merge fifth &&
	test_cmp expected file3 &&
	test_must_fail git update-index --refresh
'

test_expect_success 'rerere.autoupdate' '
	git config rerere.autoupdate true &&
	git reset --hard &&
	git checkout version2 &&
	test_must_fail git merge fifth &&
	git update-index --refresh
'

test_expect_success 'merge --rerere-autoupdate' '
	test_might_fail git config --unset rerere.autoupdate &&
	git reset --hard &&
	git checkout version2 &&
	test_must_fail git merge --rerere-autoupdate fifth &&
	git update-index --refresh
'

test_expect_success 'merge --no-rerere-autoupdate' '
	headblob=$(git rev-parse version2:file3) &&
	mergeblob=$(git rev-parse fifth:file3) &&
	cat >expected <<-EOF &&
	100644 $headblob 2	file3
	100644 $mergeblob 3	file3
	EOF

	git config rerere.autoupdate true &&
	git reset --hard &&
	git checkout version2 &&
	test_must_fail git merge --no-rerere-autoupdate fifth &&
	git ls-files -u >actual &&
	test_cmp expected actual
'

test_done
