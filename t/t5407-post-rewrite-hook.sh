#!/bin/sh
#
# Copyright (c) 2010 Thomas Rast
#

test_description='Test the post-rewrite hook.'
. ./test-lib.sh

test_expect_success 'setup' '
	test_commit A foo A &&
	test_commit B foo B &&
	test_commit C foo C &&
	test_commit D foo D
'

mkdir .git/hooks

cat >.git/hooks/post-rewrite <<EOF
#!/bin/sh
echo \$@ > "$TRASH_DIRECTORY"/post-rewrite.args
cat > "$TRASH_DIRECTORY"/post-rewrite.data
EOF
chmod u+x .git/hooks/post-rewrite

clear_hook_input () {
	rm -f post-rewrite.args post-rewrite.data
}

verify_hook_input () {
	test_cmp "$TRASH_DIRECTORY"/post-rewrite.args expected.args &&
	test_cmp "$TRASH_DIRECTORY"/post-rewrite.data expected.data
}

test_expect_success 'git commit --amend' '
	clear_hook_input &&
	echo "D new message" > newmsg &&
	oldsha=$(git rev-parse HEAD^0) &&
	git commit -Fnewmsg --amend &&
	echo amend > expected.args &&
	echo $oldsha $(git rev-parse HEAD^0) > expected.data &&
	verify_hook_input
'

test_expect_success 'git commit --amend --no-post-rewrite' '
	clear_hook_input &&
	echo "D new message again" > newmsg &&
	git commit --no-post-rewrite -Fnewmsg --amend &&
	test ! -f post-rewrite.args &&
	test ! -f post-rewrite.data
'

test_done
