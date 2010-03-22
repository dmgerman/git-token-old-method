begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"list-objects.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"log-tree.h"
end_include
begin_include
include|#
directive|include
file|"graph.h"
end_include
begin_include
include|#
directive|include
file|"bisect.h"
end_include
begin_decl_stmt
DECL|variable|rev_list_usage
specifier|static
specifier|const
name|char
name|rev_list_usage
index|[]
init|=
literal|"git rev-list [OPTION]<commit-id>... [ -- paths... ]\n"
literal|"  limiting output:\n"
literal|"    --max-count=nr\n"
literal|"    --max-age=epoch\n"
literal|"    --min-age=epoch\n"
literal|"    --sparse\n"
literal|"    --no-merges\n"
literal|"    --remove-empty\n"
literal|"    --all\n"
literal|"    --branches\n"
literal|"    --tags\n"
literal|"    --remotes\n"
literal|"    --stdin\n"
literal|"    --quiet\n"
literal|"  ordering output:\n"
literal|"    --topo-order\n"
literal|"    --date-order\n"
literal|"    --reverse\n"
literal|"  formatting output:\n"
literal|"    --parents\n"
literal|"    --children\n"
literal|"    --objects | --objects-edge\n"
literal|"    --unpacked\n"
literal|"    --header | --pretty\n"
literal|"    --abbrev=nr | --no-abbrev\n"
literal|"    --abbrev-commit\n"
literal|"    --left-right\n"
literal|"  special purpose:\n"
literal|"    --bisect\n"
literal|"    --bisect-vars\n"
literal|"    --bisect-all"
decl_stmt|;
end_decl_stmt
begin_function_decl
specifier|static
name|void
name|finish_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|show_commit
specifier|static
name|void
name|show_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|rev_list_info
modifier|*
name|info
init|=
name|data
decl_stmt|;
name|struct
name|rev_info
modifier|*
name|revs
init|=
name|info
operator|->
name|revs
decl_stmt|;
name|graph_show_commit
argument_list|(
name|revs
operator|->
name|graph
argument_list|)
expr_stmt|;
if|if
condition|(
name|info
operator|->
name|show_timestamp
condition|)
name|printf
argument_list|(
literal|"%lu "
argument_list|,
name|commit
operator|->
name|date
argument_list|)
expr_stmt|;
if|if
condition|(
name|info
operator|->
name|header_prefix
condition|)
name|fputs
argument_list|(
name|info
operator|->
name|header_prefix
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|graph
condition|)
block|{
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|BOUNDARY
condition|)
name|putchar
argument_list|(
literal|'-'
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
name|putchar
argument_list|(
literal|'^'
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|revs
operator|->
name|left_right
condition|)
block|{
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|SYMMETRIC_LEFT
condition|)
name|putchar
argument_list|(
literal|'<'
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'>'
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|revs
operator|->
name|abbrev_commit
operator|&&
name|revs
operator|->
name|abbrev
condition|)
name|fputs
argument_list|(
name|find_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|revs
operator|->
name|abbrev
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
else|else
name|fputs
argument_list|(
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|print_parents
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
if|if
condition|(
name|revs
operator|->
name|children
operator|.
name|name
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|children
decl_stmt|;
name|children
operator|=
name|lookup_decoration
argument_list|(
operator|&
name|revs
operator|->
name|children
argument_list|,
operator|&
name|commit
operator|->
name|object
argument_list|)
expr_stmt|;
while|while
condition|(
name|children
condition|)
block|{
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|children
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|children
operator|=
name|children
operator|->
name|next
expr_stmt|;
block|}
block|}
name|show_decorations
argument_list|(
name|revs
argument_list|,
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|verbose_header
operator|&&
name|commit
operator|->
name|buffer
condition|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|pretty_print_context
name|ctx
init|=
block|{
literal|0
block|}
decl_stmt|;
name|ctx
operator|.
name|abbrev
operator|=
name|revs
operator|->
name|abbrev
expr_stmt|;
name|ctx
operator|.
name|date_mode
operator|=
name|revs
operator|->
name|date_mode
expr_stmt|;
name|pretty_print_commit
argument_list|(
name|revs
operator|->
name|commit_format
argument_list|,
name|commit
argument_list|,
operator|&
name|buf
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|graph
condition|)
block|{
if|if
condition|(
name|buf
operator|.
name|len
condition|)
block|{
if|if
condition|(
name|revs
operator|->
name|commit_format
operator|!=
name|CMIT_FMT_ONELINE
condition|)
name|graph_show_oneline
argument_list|(
name|revs
operator|->
name|graph
argument_list|)
expr_stmt|;
name|graph_show_commit_msg
argument_list|(
name|revs
operator|->
name|graph
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
comment|/* 				 * Add a newline after the commit message. 				 * 				 * Usually, this newline produces a blank 				 * padding line between entries, in which case 				 * we need to add graph padding on this line. 				 * 				 * However, the commit message may not end in a 				 * newline.  In this case the newline simply 				 * ends the last line of the commit message, 				 * and we don't need any graph output.  (This 				 * always happens with CMIT_FMT_ONELINE, and it 				 * happens with CMIT_FMT_USERFORMAT when the 				 * format doesn't explicitly end in a newline.) 				 */
if|if
condition|(
name|buf
operator|.
name|len
operator|&&
name|buf
operator|.
name|buf
index|[
name|buf
operator|.
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|graph_show_padding
argument_list|(
name|revs
operator|->
name|graph
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* 				 * If the message buffer is empty, just show 				 * the rest of the graph output for this 				 * commit. 				 */
if|if
condition|(
name|graph_show_remainder
argument_list|(
name|revs
operator|->
name|graph
argument_list|)
condition|)
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
if|if
condition|(
name|buf
operator|.
name|len
condition|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|info
operator|->
name|hdr_termination
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|graph_show_remainder
argument_list|(
name|revs
operator|->
name|graph
argument_list|)
condition|)
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
name|maybe_flush_or_die
argument_list|(
name|stdout
argument_list|,
literal|"stdout"
argument_list|)
expr_stmt|;
name|finish_commit
argument_list|(
name|commit
argument_list|,
name|data
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|finish_commit
specifier|static
name|void
name|finish_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
if|if
condition|(
name|commit
operator|->
name|parents
condition|)
block|{
name|free_commit_list
argument_list|(
name|commit
operator|->
name|parents
argument_list|)
expr_stmt|;
name|commit
operator|->
name|parents
operator|=
name|NULL
expr_stmt|;
block|}
name|free
argument_list|(
name|commit
operator|->
name|buffer
argument_list|)
expr_stmt|;
name|commit
operator|->
name|buffer
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|finish_object
specifier|static
name|void
name|finish_object
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_BLOB
operator|&&
operator|!
name|has_sha1_file
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"missing blob object '%s'"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_object
specifier|static
name|void
name|show_object
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|component
parameter_list|)
block|{
name|char
modifier|*
name|name
init|=
name|path_name
argument_list|(
name|path
argument_list|,
name|component
argument_list|)
decl_stmt|;
comment|/* An object with name "foo\n0000000..." can be used to 	 * confuse downstream "git pack-objects" very badly. 	 */
specifier|const
name|char
modifier|*
name|ep
init|=
name|strchr
argument_list|(
name|name
argument_list|,
literal|'\n'
argument_list|)
decl_stmt|;
name|finish_object
argument_list|(
name|obj
argument_list|,
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|ep
condition|)
block|{
name|printf
argument_list|(
literal|"%s %.*s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|,
call|(
name|int
call|)
argument_list|(
name|ep
operator|-
name|name
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_edge
specifier|static
name|void
name|show_edge
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|printf
argument_list|(
literal|"-%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|log2i
specifier|static
specifier|inline
name|int
name|log2i
parameter_list|(
name|int
name|n
parameter_list|)
block|{
name|int
name|log2
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
name|n
operator|>
literal|1
condition|;
name|n
operator|>>=
literal|1
control|)
name|log2
operator|++
expr_stmt|;
return|return
name|log2
return|;
block|}
end_function
begin_function
DECL|function|exp2i
specifier|static
specifier|inline
name|int
name|exp2i
parameter_list|(
name|int
name|n
parameter_list|)
block|{
return|return
literal|1
operator|<<
name|n
return|;
block|}
end_function
begin_comment
comment|/*  * Estimate the number of bisect steps left (after the current step)  *  * For any x between 0 included and 2^n excluded, the probability for  * n - 1 steps left looks like:  *  * P(2^n + x) == (2^n - x) / (2^n + x)  *  * and P(2^n + x)< 0.5 means 2^n< 3x  */
end_comment
begin_function
DECL|function|estimate_bisect_steps
name|int
name|estimate_bisect_steps
parameter_list|(
name|int
name|all
parameter_list|)
block|{
name|int
name|n
decl_stmt|,
name|x
decl_stmt|,
name|e
decl_stmt|;
if|if
condition|(
name|all
operator|<
literal|3
condition|)
return|return
literal|0
return|;
name|n
operator|=
name|log2i
argument_list|(
name|all
argument_list|)
expr_stmt|;
name|e
operator|=
name|exp2i
argument_list|(
name|n
argument_list|)
expr_stmt|;
name|x
operator|=
name|all
operator|-
name|e
expr_stmt|;
return|return
operator|(
name|e
operator|<
literal|3
operator|*
name|x
operator|)
condition|?
name|n
else|:
name|n
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|print_commit_list
name|void
name|print_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|format_cur
parameter_list|,
specifier|const
name|char
modifier|*
name|format_last
parameter_list|)
block|{
for|for
control|(
init|;
name|list
condition|;
name|list
operator|=
name|list
operator|->
name|next
control|)
block|{
specifier|const
name|char
modifier|*
name|format
init|=
name|list
operator|->
name|next
condition|?
name|format_cur
else|:
name|format_last
decl_stmt|;
name|printf
argument_list|(
name|format
argument_list|,
name|sha1_to_hex
argument_list|(
name|list
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_tried_revs
specifier|static
name|void
name|show_tried_revs
parameter_list|(
name|struct
name|commit_list
modifier|*
name|tried
parameter_list|)
block|{
name|printf
argument_list|(
literal|"bisect_tried='"
argument_list|)
expr_stmt|;
name|print_commit_list
argument_list|(
name|tried
argument_list|,
literal|"%s|"
argument_list|,
literal|"%s"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"'\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|print_var_str
specifier|static
name|void
name|print_var_str
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|val
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s='%s'\n"
argument_list|,
name|var
argument_list|,
name|val
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|print_var_int
specifier|static
name|void
name|print_var_int
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
name|int
name|val
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s=%d\n"
argument_list|,
name|var
argument_list|,
name|val
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_bisect_vars
specifier|static
name|int
name|show_bisect_vars
parameter_list|(
name|struct
name|rev_list_info
modifier|*
name|info
parameter_list|,
name|int
name|reaches
parameter_list|,
name|int
name|all
parameter_list|)
block|{
name|int
name|cnt
decl_stmt|,
name|flags
init|=
name|info
operator|->
name|bisect_show_flags
decl_stmt|;
name|char
name|hex
index|[
literal|41
index|]
init|=
literal|""
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|tried
decl_stmt|;
name|struct
name|rev_info
modifier|*
name|revs
init|=
name|info
operator|->
name|revs
decl_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|commits
operator|&&
operator|!
operator|(
name|flags
operator|&
name|BISECT_SHOW_TRIED
operator|)
condition|)
return|return
literal|1
return|;
name|revs
operator|->
name|commits
operator|=
name|filter_skipped
argument_list|(
name|revs
operator|->
name|commits
argument_list|,
operator|&
name|tried
argument_list|,
name|flags
operator|&
name|BISECT_SHOW_ALL
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
comment|/* 	 * revs->commits can reach "reaches" commits among 	 * "all" commits.  If it is good, then there are 	 * (all-reaches) commits left to be bisected. 	 * On the other hand, if it is bad, then the set 	 * to bisect is "reaches". 	 * A bisect set of size N has (N-1) commits further 	 * to test, as we already know one bad one. 	 */
name|cnt
operator|=
name|all
operator|-
name|reaches
expr_stmt|;
if|if
condition|(
name|cnt
operator|<
name|reaches
condition|)
name|cnt
operator|=
name|reaches
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|commits
condition|)
name|strcpy
argument_list|(
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|revs
operator|->
name|commits
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|flags
operator|&
name|BISECT_SHOW_ALL
condition|)
block|{
name|traverse_commit_list
argument_list|(
name|revs
argument_list|,
name|show_commit
argument_list|,
name|show_object
argument_list|,
name|info
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"------\n"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|flags
operator|&
name|BISECT_SHOW_TRIED
condition|)
name|show_tried_revs
argument_list|(
name|tried
argument_list|)
expr_stmt|;
name|print_var_str
argument_list|(
literal|"bisect_rev"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
name|print_var_int
argument_list|(
literal|"bisect_nr"
argument_list|,
name|cnt
operator|-
literal|1
argument_list|)
expr_stmt|;
name|print_var_int
argument_list|(
literal|"bisect_good"
argument_list|,
name|all
operator|-
name|reaches
operator|-
literal|1
argument_list|)
expr_stmt|;
name|print_var_int
argument_list|(
literal|"bisect_bad"
argument_list|,
name|reaches
operator|-
literal|1
argument_list|)
expr_stmt|;
name|print_var_int
argument_list|(
literal|"bisect_all"
argument_list|,
name|all
argument_list|)
expr_stmt|;
name|print_var_int
argument_list|(
literal|"bisect_steps"
argument_list|,
name|estimate_bisect_steps
argument_list|(
name|all
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_rev_list
name|int
name|cmd_rev_list
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
name|struct
name|rev_info
name|revs
decl_stmt|;
name|struct
name|rev_list_info
name|info
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|bisect_list
init|=
literal|0
decl_stmt|;
name|int
name|bisect_show_vars
init|=
literal|0
decl_stmt|;
name|int
name|bisect_find_all
init|=
literal|0
decl_stmt|;
name|int
name|quiet
init|=
literal|0
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|revs
operator|.
name|abbrev
operator|=
name|DEFAULT_ABBREV
expr_stmt|;
name|revs
operator|.
name|commit_format
operator|=
name|CMIT_FMT_UNSPECIFIED
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|info
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|info
argument_list|)
argument_list|)
expr_stmt|;
name|info
operator|.
name|revs
operator|=
operator|&
name|revs
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|bisect
condition|)
name|bisect_list
operator|=
literal|1
expr_stmt|;
name|quiet
operator|=
name|DIFF_OPT_TST
argument_list|(
operator|&
name|revs
operator|.
name|diffopt
argument_list|,
name|QUICK
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--header"
argument_list|)
condition|)
block|{
name|revs
operator|.
name|verbose_header
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--timestamp"
argument_list|)
condition|)
block|{
name|info
operator|.
name|show_timestamp
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--bisect"
argument_list|)
condition|)
block|{
name|bisect_list
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--bisect-all"
argument_list|)
condition|)
block|{
name|bisect_list
operator|=
literal|1
expr_stmt|;
name|bisect_find_all
operator|=
literal|1
expr_stmt|;
name|info
operator|.
name|bisect_show_flags
operator|=
name|BISECT_SHOW_ALL
expr_stmt|;
name|revs
operator|.
name|show_decorations
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--bisect-vars"
argument_list|)
condition|)
block|{
name|bisect_list
operator|=
literal|1
expr_stmt|;
name|bisect_show_vars
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|revs
operator|.
name|commit_format
operator|!=
name|CMIT_FMT_UNSPECIFIED
condition|)
block|{
comment|/* The command line has a --pretty  */
name|info
operator|.
name|hdr_termination
operator|=
literal|'\n'
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
name|info
operator|.
name|header_prefix
operator|=
literal|""
expr_stmt|;
else|else
name|info
operator|.
name|header_prefix
operator|=
literal|"commit "
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|revs
operator|.
name|verbose_header
condition|)
comment|/* Only --header was specified */
name|revs
operator|.
name|commit_format
operator|=
name|CMIT_FMT_RAW
expr_stmt|;
if|if
condition|(
operator|(
operator|!
name|revs
operator|.
name|commits
operator|&&
operator|(
operator|!
operator|(
name|revs
operator|.
name|tag_objects
operator|||
name|revs
operator|.
name|tree_objects
operator|||
name|revs
operator|.
name|blob_objects
operator|)
operator|&&
operator|!
name|revs
operator|.
name|pending
operator|.
name|nr
operator|)
operator|)
operator|||
name|revs
operator|.
name|diff
condition|)
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
name|save_commit_buffer
operator|=
operator|(
name|revs
operator|.
name|verbose_header
operator|||
name|revs
operator|.
name|grep_filter
operator|.
name|pattern_list
operator|||
name|revs
operator|.
name|grep_filter
operator|.
name|header_list
operator|)
expr_stmt|;
if|if
condition|(
name|bisect_list
condition|)
name|revs
operator|.
name|limited
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|prepare_revision_walk
argument_list|(
operator|&
name|revs
argument_list|)
condition|)
name|die
argument_list|(
literal|"revision walk setup failed"
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|tree_objects
condition|)
name|mark_edges_uninteresting
argument_list|(
name|revs
operator|.
name|commits
argument_list|,
operator|&
name|revs
argument_list|,
name|show_edge
argument_list|)
expr_stmt|;
if|if
condition|(
name|bisect_list
condition|)
block|{
name|int
name|reaches
init|=
name|reaches
decl_stmt|,
name|all
init|=
name|all
decl_stmt|;
name|revs
operator|.
name|commits
operator|=
name|find_bisection
argument_list|(
name|revs
operator|.
name|commits
argument_list|,
operator|&
name|reaches
argument_list|,
operator|&
name|all
argument_list|,
name|bisect_find_all
argument_list|)
expr_stmt|;
if|if
condition|(
name|bisect_show_vars
condition|)
return|return
name|show_bisect_vars
argument_list|(
operator|&
name|info
argument_list|,
name|reaches
argument_list|,
name|all
argument_list|)
return|;
block|}
name|traverse_commit_list
argument_list|(
operator|&
name|revs
argument_list|,
name|quiet
condition|?
name|finish_commit
else|:
name|show_commit
argument_list|,
name|quiet
condition|?
name|finish_object
else|:
name|show_object
argument_list|,
operator|&
name|info
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
