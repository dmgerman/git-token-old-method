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
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"ref-filter.h"
end_include
begin_decl_stmt
DECL|variable|for_each_ref_usage
specifier|static
name|char
specifier|const
modifier|*
specifier|const
name|for_each_ref_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git for-each-ref [<options>] [<pattern>]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_for_each_ref
name|int
name|cmd_for_each_ref
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
name|int
name|i
decl_stmt|;
specifier|const
name|char
modifier|*
name|format
init|=
literal|"%(objectname) %(objecttype)\t%(refname)"
decl_stmt|;
name|struct
name|ref_sorting
modifier|*
name|sorting
init|=
name|NULL
decl_stmt|,
modifier|*
modifier|*
name|sorting_tail
init|=
operator|&
name|sorting
decl_stmt|;
name|int
name|maxcount
init|=
literal|0
decl_stmt|,
name|quote_style
init|=
literal|0
decl_stmt|;
name|struct
name|ref_array
name|array
decl_stmt|;
name|struct
name|ref_filter
name|filter
decl_stmt|;
name|struct
name|option
name|opts
index|[]
init|=
block|{
name|OPT_BIT
argument_list|(
literal|'s'
argument_list|,
literal|"shell"
argument_list|,
operator|&
name|quote_style
argument_list|,
name|N_
argument_list|(
literal|"quote placeholders suitably for shells"
argument_list|)
argument_list|,
name|QUOTE_SHELL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'p'
argument_list|,
literal|"perl"
argument_list|,
operator|&
name|quote_style
argument_list|,
name|N_
argument_list|(
literal|"quote placeholders suitably for perl"
argument_list|)
argument_list|,
name|QUOTE_PERL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"python"
argument_list|,
operator|&
name|quote_style
argument_list|,
name|N_
argument_list|(
literal|"quote placeholders suitably for python"
argument_list|)
argument_list|,
name|QUOTE_PYTHON
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"tcl"
argument_list|,
operator|&
name|quote_style
argument_list|,
name|N_
argument_list|(
literal|"quote placeholders suitably for Tcl"
argument_list|)
argument_list|,
name|QUOTE_TCL
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|""
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"count"
argument_list|,
operator|&
name|maxcount
argument_list|,
name|N_
argument_list|(
literal|"show only<n> matched refs"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"format"
argument_list|,
operator|&
name|format
argument_list|,
name|N_
argument_list|(
literal|"format"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"format to use for the output"
argument_list|)
argument_list|)
block|,
name|OPT_CALLBACK
argument_list|(
literal|0
argument_list|,
literal|"sort"
argument_list|,
name|sorting_tail
argument_list|,
name|N_
argument_list|(
literal|"key"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"field name to sort on"
argument_list|)
argument_list|,
operator|&
name|parse_opt_ref_sorting
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|opts
argument_list|,
name|for_each_ref_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|maxcount
operator|<
literal|0
condition|)
block|{
name|error
argument_list|(
literal|"invalid --count argument: `%d'"
argument_list|,
name|maxcount
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|for_each_ref_usage
argument_list|,
name|opts
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|HAS_MULTI_BITS
argument_list|(
name|quote_style
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"more than one quoting style?"
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|for_each_ref_usage
argument_list|,
name|opts
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|verify_ref_format
argument_list|(
name|format
argument_list|)
condition|)
name|usage_with_options
argument_list|(
name|for_each_ref_usage
argument_list|,
name|opts
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sorting
condition|)
name|sorting
operator|=
name|ref_default_sorting
argument_list|()
expr_stmt|;
comment|/* for warn_ambiguous_refs */
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|array
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|array
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|filter
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|filter
argument_list|)
argument_list|)
expr_stmt|;
name|filter
operator|.
name|name_patterns
operator|=
name|argv
expr_stmt|;
name|filter_refs
argument_list|(
operator|&
name|array
argument_list|,
operator|&
name|filter
argument_list|,
name|FILTER_REFS_ALL
operator||
name|FILTER_REFS_INCLUDE_BROKEN
argument_list|)
expr_stmt|;
name|ref_array_sort
argument_list|(
name|sorting
argument_list|,
operator|&
name|array
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|maxcount
operator|||
name|array
operator|.
name|nr
operator|<
name|maxcount
condition|)
name|maxcount
operator|=
name|array
operator|.
name|nr
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|maxcount
condition|;
name|i
operator|++
control|)
name|show_ref_array_item
argument_list|(
name|array
operator|.
name|items
index|[
name|i
index|]
argument_list|,
name|format
argument_list|,
name|quote_style
argument_list|)
expr_stmt|;
name|ref_array_clear
argument_list|(
operator|&
name|array
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
