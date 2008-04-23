#!/bin/sh

test_description='git cat-file'

. ./test-lib.sh

echo_without_newline () {
    printf '%s' "$*"
}

strlen () {
    echo_without_newline "$1" | wc -c | sed -e 's/^ *//'
}

maybe_remove_timestamp () {
    if test -z "$2"; then
        echo_without_newline "$1"
    else
	echo_without_newline "$(printf '%s\n' "$1" | sed -e 's/ [0-9][0-9]* [-+][0-9][0-9][0-9][0-9]$//')"
    fi
}

run_tests () {
    type=$1
    sha1=$2
    size=$3
    content=$4
    pretty_content=$5
    no_ts=$6

    test_expect_success "$type exists" '
	git cat-file -e $sha1
    '

    test_expect_success "Type of $type is correct" '
        test $type = "$(git cat-file -t $sha1)"
    '

    test_expect_success "Size of $type is correct" '
        test $size = "$(git cat-file -s $sha1)"
    '

    test -z "$content" ||
    test_expect_success "Content of $type is correct" '
	expect="$(maybe_remove_timestamp "$content" $no_ts)"
	actual="$(maybe_remove_timestamp "$(git cat-file $type $sha1)" $no_ts)"

        if test "z$expect" = "z$actual"
	then
		: happy
	else
		echo "Oops: expected $expect"
		echo "but got $actual"
		false
        fi
    '

    test_expect_success "Pretty content of $type is correct" '
	expect="$(maybe_remove_timestamp "$pretty_content" $no_ts)"
	actual="$(maybe_remove_timestamp "$(git cat-file -p $sha1)" $no_ts)"
        if test "z$expect" = "z$actual"
	then
		: happy
	else
		echo "Oops: expected $expect"
		echo "but got $actual"
		false
        fi
    '

    test_expect_success "--batch-check output of $type is correct" '
	expect="$sha1 $type $size"
	actual="$(echo_without_newline $sha1 | git cat-file --batch-check)"
        if test "z$expect" = "z$actual"
	then
		: happy
	else
		echo "Oops: expected $expect"
		echo "but got $actual"
		false
        fi
    '
}

hello_content="Hello World"
hello_size=$(strlen "$hello_content")
hello_sha1=$(echo_without_newline "$hello_content" | git hash-object --stdin)

test_expect_success "setup" '
	echo_without_newline "$hello_content" > hello &&
	git update-index --add hello
'

run_tests 'blob' $hello_sha1 $hello_size "$hello_content" "$hello_content"

tree_sha1=$(git write-tree)
tree_size=33
tree_pretty_content="100644 blob $hello_sha1	hello"

run_tests 'tree' $tree_sha1 $tree_size "" "$tree_pretty_content"

commit_message="Intial commit"
commit_sha1=$(echo_without_newline "$commit_message" | git commit-tree $tree_sha1)
commit_size=176
commit_content="tree $tree_sha1
author $GIT_AUTHOR_NAME <$GIT_AUTHOR_EMAIL> 0000000000 +0000
committer $GIT_COMMITTER_NAME <$GIT_COMMITTER_EMAIL> 0000000000 +0000

$commit_message"

run_tests 'commit' $commit_sha1 $commit_size "$commit_content" "$commit_content" 1

tag_header_without_timestamp="object $hello_sha1
type blob
tag hellotag
tagger $GIT_COMMITTER_NAME <$GIT_COMMITTER_EMAIL>"
tag_description="This is a tag"
tag_content="$tag_header_without_timestamp 0000000000 +0000

$tag_description"
tag_pretty_content="$tag_header_without_timestamp Thu Jan 1 00:00:00 1970 +0000

$tag_description"

tag_sha1=$(echo_without_newline "$tag_content" | git mktag)
tag_size=$(strlen "$tag_content")

run_tests 'tag' $tag_sha1 $tag_size "$tag_content" "$tag_pretty_content" 1

test_expect_success \
    "Reach a blob from a tag pointing to it" \
    "test '$hello_content' = \"\$(git cat-file blob $tag_sha1)\""

for opt in t s e p
do
    test_expect_success "Passing -$opt with --batch-check fails" '
	test_must_fail git cat-file --batch-check -$opt $hello_sha1
    '

    test_expect_success "Passing --batch-check with -$opt fails" '
	test_must_fail git cat-file -$opt --batch-check $hello_sha1
    '
done

test_expect_success "Passing <type> with --batch-check fails" '
    test_must_fail git cat-file --batch-check blob $hello_sha1
'

test_expect_success "Passing --batch-check with <type> fails" '
    test_must_fail git cat-file blob --batch-check $hello_sha1
'

test_expect_success "Passing sha1 with --batch-check fails" '
    test_must_fail git cat-file --batch-check $hello_sha1
'

test_expect_success "--batch-check for a non-existent object" '
    test "deadbeef missing" = \
    "$(echo_without_newline deadbeef | git cat-file --batch-check)"
'

test_expect_success "--batch-check for an emtpy line" '
    test " missing" = "$(echo | git cat-file --batch-check)"
'

batch_check_input="$hello_sha1
$tree_sha1
$commit_sha1
$tag_sha1
deadbeef

"

batch_check_output="$hello_sha1 blob $hello_size
$tree_sha1 tree $tree_size
$commit_sha1 commit $commit_size
$tag_sha1 tag $tag_size
deadbeef missing
 missing"

test_expect_success "--batch-check with multiple sha1s gives correct format" '
    test "$batch_check_output" = \
    "$(echo_without_newline "$batch_check_input" | git cat-file --batch-check)"
'

test_done
