begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|show_ref_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|show_ref_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git show-ref [-q|--quiet] [--verify] [--head] [-d|--dereference] [-s|--hash[=<n>]] [--abbrev[=<n>]] [--tags] [--heads] [--] [pattern*] "
argument_list|)
block|,
name|N_
argument_list|(
literal|"git show-ref --exclude-existing[=pattern]< ref-list"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|deref_tags
DECL|variable|show_head
DECL|variable|tags_only
DECL|variable|heads_only
DECL|variable|found_match
DECL|variable|verify
specifier|static
name|int
name|deref_tags
decl_stmt|,
name|show_head
decl_stmt|,
name|tags_only
decl_stmt|,
name|heads_only
decl_stmt|,
name|found_match
decl_stmt|,
name|verify
decl_stmt|,
DECL|variable|quiet
DECL|variable|hash_only
DECL|variable|abbrev
DECL|variable|exclude_arg
name|quiet
decl_stmt|,
name|hash_only
decl_stmt|,
name|abbrev
decl_stmt|,
name|exclude_arg
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pattern
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|pattern
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|exclude_existing_arg
specifier|static
specifier|const
name|char
modifier|*
name|exclude_existing_arg
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|show_one
specifier|static
name|void
name|show_one
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|hex
init|=
name|find_unique_abbrev
argument_list|(
name|sha1
argument_list|,
name|abbrev
argument_list|)
decl_stmt|;
if|if
condition|(
name|hash_only
condition|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|hex
argument_list|,
name|refname
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_ref
specifier|static
name|int
name|show_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cbdata
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|hex
decl_stmt|;
name|unsigned
name|char
name|peeled
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|tags_only
operator|||
name|heads_only
condition|)
block|{
name|int
name|match
decl_stmt|;
name|match
operator|=
name|heads_only
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|refname
argument_list|,
literal|"refs/heads/"
argument_list|)
expr_stmt|;
name|match
operator||=
name|tags_only
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|refname
argument_list|,
literal|"refs/tags/"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|match
condition|)
return|return
literal|0
return|;
block|}
if|if
condition|(
name|pattern
condition|)
block|{
name|int
name|reflen
init|=
name|strlen
argument_list|(
name|refname
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
init|=
name|pattern
decl_stmt|,
modifier|*
name|m
decl_stmt|;
while|while
condition|(
operator|(
name|m
operator|=
operator|*
name|p
operator|++
operator|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|m
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|>
name|reflen
condition|)
continue|continue;
if|if
condition|(
name|memcmp
argument_list|(
name|m
argument_list|,
name|refname
operator|+
name|reflen
operator|-
name|len
argument_list|,
name|len
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|len
operator|==
name|reflen
condition|)
goto|goto
name|match
goto|;
comment|/* "--verify" requires an exact match */
if|if
condition|(
name|verify
condition|)
continue|continue;
if|if
condition|(
name|refname
index|[
name|reflen
operator|-
name|len
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
goto|goto
name|match
goto|;
block|}
return|return
literal|0
return|;
block|}
name|match
label|:
name|found_match
operator|++
expr_stmt|;
comment|/* This changes the semantics slightly that even under quiet we 	 * detect and return error if the repository is corrupt and 	 * ref points at a nonexistent object. 	 */
if|if
condition|(
operator|!
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"git show-ref: bad ref %s (%s)"
argument_list|,
name|refname
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|quiet
condition|)
return|return
literal|0
return|;
name|show_one
argument_list|(
name|refname
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|deref_tags
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|peel_ref
argument_list|(
name|refname
argument_list|,
name|peeled
argument_list|)
condition|)
block|{
name|hex
operator|=
name|find_unique_abbrev
argument_list|(
name|peeled
argument_list|,
name|abbrev
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s %s^{}\n"
argument_list|,
name|hex
argument_list|,
name|refname
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_existing
specifier|static
name|int
name|add_existing
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cbdata
parameter_list|)
block|{
name|struct
name|string_list
modifier|*
name|list
init|=
operator|(
expr|struct
name|string_list
operator|*
operator|)
name|cbdata
decl_stmt|;
name|string_list_insert
argument_list|(
name|list
argument_list|,
name|refname
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * read "^(?:<anything>\s)?<refname>(?:\^\{\})?$" from the standard input,  * and  * (1) strip "^{}" at the end of line if any;  * (2) ignore if match is provided and does not head-match refname;  * (3) warn if refname is not a well-formed refname and skip;  * (4) ignore if refname is a ref that exists in the local repository;  * (5) otherwise output the line.  */
end_comment
begin_function
DECL|function|exclude_existing
specifier|static
name|int
name|exclude_existing
parameter_list|(
specifier|const
name|char
modifier|*
name|match
parameter_list|)
block|{
specifier|static
name|struct
name|string_list
name|existing_refs
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
name|int
name|matchlen
init|=
name|match
condition|?
name|strlen
argument_list|(
name|match
argument_list|)
else|:
literal|0
decl_stmt|;
name|for_each_ref
argument_list|(
name|add_existing
argument_list|,
operator|&
name|existing_refs
argument_list|)
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|stdin
argument_list|)
condition|)
block|{
name|char
modifier|*
name|ref
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|buf
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|>
literal|0
operator|&&
name|buf
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buf
index|[
operator|--
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
literal|3
operator|<=
name|len
operator|&&
operator|!
name|strcmp
argument_list|(
name|buf
operator|+
name|len
operator|-
literal|3
argument_list|,
literal|"^{}"
argument_list|)
condition|)
block|{
name|len
operator|-=
literal|3
expr_stmt|;
name|buf
index|[
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
for|for
control|(
name|ref
operator|=
name|buf
operator|+
name|len
init|;
name|buf
operator|<
name|ref
condition|;
name|ref
operator|--
control|)
if|if
condition|(
name|isspace
argument_list|(
name|ref
index|[
operator|-
literal|1
index|]
argument_list|)
condition|)
break|break;
if|if
condition|(
name|match
condition|)
block|{
name|int
name|reflen
init|=
name|buf
operator|+
name|len
operator|-
name|ref
decl_stmt|;
if|if
condition|(
name|reflen
operator|<
name|matchlen
condition|)
continue|continue;
if|if
condition|(
name|strncmp
argument_list|(
name|ref
argument_list|,
name|match
argument_list|,
name|matchlen
argument_list|)
condition|)
continue|continue;
block|}
if|if
condition|(
name|check_refname_format
argument_list|(
name|ref
argument_list|,
literal|0
argument_list|)
condition|)
block|{
name|warning
argument_list|(
literal|"ref '%s' ignored"
argument_list|,
name|ref
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|string_list_has_string
argument_list|(
operator|&
name|existing_refs
argument_list|,
name|ref
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|hash_callback
specifier|static
name|int
name|hash_callback
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|hash_only
operator|=
literal|1
expr_stmt|;
comment|/* Use full length SHA1 if no argument */
if|if
condition|(
operator|!
name|arg
condition|)
return|return
literal|0
return|;
return|return
name|parse_opt_abbrev_cb
argument_list|(
name|opt
argument_list|,
name|arg
argument_list|,
name|unset
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|exclude_existing_callback
specifier|static
name|int
name|exclude_existing_callback
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|exclude_arg
operator|=
literal|1
expr_stmt|;
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
name|arg
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|help_callback
specifier|static
name|int
name|help_callback
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|show_ref_options
specifier|static
specifier|const
name|struct
name|option
name|show_ref_options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"tags"
argument_list|,
operator|&
name|tags_only
argument_list|,
name|N_
argument_list|(
literal|"only show tags (can be combined with heads)"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"heads"
argument_list|,
operator|&
name|heads_only
argument_list|,
name|N_
argument_list|(
literal|"only show heads (can be combined with tags)"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"verify"
argument_list|,
operator|&
name|verify
argument_list|,
name|N_
argument_list|(
literal|"stricter reference checking, "
literal|"requires exact ref path"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_BOOLEAN
block|,
literal|'h'
block|,
name|NULL
block|,
operator|&
name|show_head
block|,
name|NULL
block|,
name|N_
argument_list|(
literal|"show the HEAD reference"
argument_list|)
block|,
name|PARSE_OPT_NOARG
operator||
name|PARSE_OPT_HIDDEN
block|}
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"head"
argument_list|,
operator|&
name|show_head
argument_list|,
name|N_
argument_list|(
literal|"show the HEAD reference"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'d'
argument_list|,
literal|"dereference"
argument_list|,
operator|&
name|deref_tags
argument_list|,
name|N_
argument_list|(
literal|"dereference tags into object IDs"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'s'
block|,
literal|"hash"
block|,
operator|&
name|abbrev
block|,
name|N_
argument_list|(
literal|"n"
argument_list|)
block|,
name|N_
argument_list|(
literal|"only show SHA1 hash using<n> digits"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
block|,
operator|&
name|hash_callback
block|}
block|,
name|OPT__ABBREV
argument_list|(
operator|&
name|abbrev
argument_list|)
block|,
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
name|N_
argument_list|(
literal|"do not print results to stdout (useful with --verify)"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
literal|"exclude-existing"
block|,
operator|&
name|exclude_existing_arg
block|,
name|N_
argument_list|(
literal|"pattern"
argument_list|)
block|,
name|N_
argument_list|(
literal|"show refs from stdin that aren't in local repository"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
operator||
name|PARSE_OPT_NONEG
block|,
name|exclude_existing_callback
block|}
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
literal|"help-all"
block|,
name|NULL
block|,
name|NULL
block|,
name|N_
argument_list|(
literal|"show usage"
argument_list|)
block|,
name|PARSE_OPT_HIDDEN
operator||
name|PARSE_OPT_NOARG
block|,
name|help_callback
block|}
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_show_ref
name|int
name|cmd_show_ref
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
if|if
condition|(
name|argc
operator|==
literal|2
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
condition|)
name|usage_with_options
argument_list|(
name|show_ref_usage
argument_list|,
name|show_ref_options
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|show_ref_options
argument_list|,
name|show_ref_usage
argument_list|,
name|PARSE_OPT_NO_INTERNAL_HELP
argument_list|)
expr_stmt|;
if|if
condition|(
name|exclude_arg
condition|)
return|return
name|exclude_existing
argument_list|(
name|exclude_existing_arg
argument_list|)
return|;
name|pattern
operator|=
name|argv
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|pattern
condition|)
name|pattern
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|verify
condition|)
block|{
if|if
condition|(
operator|!
name|pattern
condition|)
name|die
argument_list|(
literal|"--verify requires a reference"
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|pattern
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
operator|*
name|pattern
argument_list|,
literal|"refs/"
argument_list|)
operator|&&
operator|!
name|read_ref
argument_list|(
operator|*
name|pattern
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|show_one
argument_list|(
operator|*
name|pattern
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|quiet
condition|)
name|die
argument_list|(
literal|"'%s' - not a valid ref"
argument_list|,
operator|*
name|pattern
argument_list|)
expr_stmt|;
else|else
return|return
literal|1
return|;
name|pattern
operator|++
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|if
condition|(
name|show_head
condition|)
name|head_ref
argument_list|(
name|show_ref
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|for_each_ref
argument_list|(
name|show_ref
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|found_match
condition|)
block|{
if|if
condition|(
name|verify
operator|&&
operator|!
name|quiet
condition|)
name|die
argument_list|(
literal|"No match"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
