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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_decl_stmt
DECL|variable|name_rev_usage
specifier|static
specifier|const
name|char
name|name_rev_usage
index|[]
init|=
literal|"git-name-rev [--tags | --refs=<pattern>] ( --all | --stdin | committish [committish...] )\n"
decl_stmt|;
end_decl_stmt
begin_typedef
DECL|struct|rev_name
typedef|typedef
struct|struct
name|rev_name
block|{
DECL|member|tip_name
specifier|const
name|char
modifier|*
name|tip_name
decl_stmt|;
DECL|member|merge_traversals
name|int
name|merge_traversals
decl_stmt|;
DECL|member|generation
name|int
name|generation
decl_stmt|;
block|}
DECL|typedef|rev_name
name|rev_name
typedef|;
end_typedef
begin_decl_stmt
DECL|variable|cutoff
specifier|static
name|long
name|cutoff
init|=
name|LONG_MAX
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|name_rev
specifier|static
name|void
name|name_rev
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|tip_name
parameter_list|,
name|int
name|merge_traversals
parameter_list|,
name|int
name|generation
parameter_list|,
name|int
name|deref
parameter_list|)
block|{
name|struct
name|rev_name
modifier|*
name|name
init|=
operator|(
expr|struct
name|rev_name
operator|*
operator|)
name|commit
operator|->
name|util
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|int
name|parent_number
init|=
literal|1
decl_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|object
operator|.
name|parsed
condition|)
name|parse_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|date
operator|<
name|cutoff
condition|)
return|return;
if|if
condition|(
name|deref
condition|)
block|{
name|char
modifier|*
name|new_name
init|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|tip_name
argument_list|)
operator|+
literal|3
argument_list|)
decl_stmt|;
name|strcpy
argument_list|(
name|new_name
argument_list|,
name|tip_name
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|new_name
argument_list|,
literal|"^0"
argument_list|)
expr_stmt|;
name|tip_name
operator|=
name|new_name
expr_stmt|;
if|if
condition|(
name|generation
condition|)
name|die
argument_list|(
literal|"generation: %d, but deref?"
argument_list|,
name|generation
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|name
operator|==
name|NULL
condition|)
block|{
name|name
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|rev_name
argument_list|)
argument_list|)
expr_stmt|;
name|commit
operator|->
name|util
operator|=
name|name
expr_stmt|;
goto|goto
name|copy_data
goto|;
block|}
elseif|else
if|if
condition|(
name|name
operator|->
name|merge_traversals
operator|>
name|merge_traversals
operator|||
operator|(
name|name
operator|->
name|merge_traversals
operator|==
name|merge_traversals
operator|&&
name|name
operator|->
name|generation
operator|>
name|generation
operator|)
condition|)
block|{
name|copy_data
label|:
name|name
operator|->
name|tip_name
operator|=
name|tip_name
expr_stmt|;
name|name
operator|->
name|merge_traversals
operator|=
name|merge_traversals
expr_stmt|;
name|name
operator|->
name|generation
operator|=
name|generation
expr_stmt|;
block|}
else|else
return|return;
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
operator|,
name|parent_number
operator|++
control|)
block|{
if|if
condition|(
name|parent_number
operator|>
literal|1
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|tip_name
argument_list|)
decl_stmt|;
name|char
modifier|*
name|new_name
init|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|1
operator|+
name|decimal_length
argument_list|(
name|generation
argument_list|)
operator|+
comment|/* ~<n> */
literal|1
operator|+
literal|2
operator|+
comment|/* ^NN */
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|>
literal|2
operator|&&
operator|!
name|strcmp
argument_list|(
name|tip_name
operator|+
name|len
operator|-
literal|2
argument_list|,
literal|"^0"
argument_list|)
condition|)
name|len
operator|-=
literal|2
expr_stmt|;
if|if
condition|(
name|generation
operator|>
literal|0
condition|)
name|sprintf
argument_list|(
name|new_name
argument_list|,
literal|"%.*s~%d^%d"
argument_list|,
name|len
argument_list|,
name|tip_name
argument_list|,
name|generation
argument_list|,
name|parent_number
argument_list|)
expr_stmt|;
else|else
name|sprintf
argument_list|(
name|new_name
argument_list|,
literal|"%.*s^%d"
argument_list|,
name|len
argument_list|,
name|tip_name
argument_list|,
name|parent_number
argument_list|)
expr_stmt|;
name|name_rev
argument_list|(
name|parents
operator|->
name|item
argument_list|,
name|new_name
argument_list|,
name|merge_traversals
operator|+
literal|1
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|name_rev
argument_list|(
name|parents
operator|->
name|item
argument_list|,
name|tip_name
argument_list|,
name|merge_traversals
argument_list|,
name|generation
operator|+
literal|1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_struct
DECL|struct|name_ref_data
struct|struct
name|name_ref_data
block|{
DECL|member|tags_only
name|int
name|tags_only
decl_stmt|;
DECL|member|name_only
name|int
name|name_only
decl_stmt|;
DECL|member|ref_filter
specifier|const
name|char
modifier|*
name|ref_filter
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|name_ref
specifier|static
name|int
name|name_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flags
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|name_ref_data
modifier|*
name|data
init|=
name|cb_data
decl_stmt|;
name|int
name|deref
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|data
operator|->
name|tags_only
operator|&&
name|prefixcmp
argument_list|(
name|path
argument_list|,
literal|"refs/tags/"
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|data
operator|->
name|ref_filter
operator|&&
name|fnmatch
argument_list|(
name|data
operator|->
name|ref_filter
argument_list|,
name|path
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
name|o
operator|&&
name|o
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
name|struct
name|tag
modifier|*
name|t
init|=
operator|(
expr|struct
name|tag
operator|*
operator|)
name|o
decl_stmt|;
if|if
condition|(
operator|!
name|t
operator|->
name|tagged
condition|)
break|break;
comment|/* broken repository */
name|o
operator|=
name|parse_object
argument_list|(
name|t
operator|->
name|tagged
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|deref
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|o
operator|&&
name|o
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
decl_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|path
argument_list|,
literal|"refs/heads/"
argument_list|)
condition|)
name|path
operator|=
name|path
operator|+
literal|11
expr_stmt|;
elseif|else
if|if
condition|(
name|data
operator|->
name|tags_only
operator|&&
name|data
operator|->
name|name_only
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|path
argument_list|,
literal|"refs/tags/"
argument_list|)
condition|)
name|path
operator|=
name|path
operator|+
literal|10
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|path
argument_list|,
literal|"refs/"
argument_list|)
condition|)
name|path
operator|=
name|path
operator|+
literal|5
expr_stmt|;
name|name_rev
argument_list|(
name|commit
argument_list|,
name|xstrdup
argument_list|(
name|path
argument_list|)
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|,
name|deref
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* returns a static buffer */
end_comment
begin_function
DECL|function|get_rev_name
specifier|static
specifier|const
name|char
modifier|*
name|get_rev_name
parameter_list|(
name|struct
name|object
modifier|*
name|o
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|struct
name|rev_name
modifier|*
name|n
decl_stmt|;
name|struct
name|commit
modifier|*
name|c
decl_stmt|;
if|if
condition|(
name|o
operator|->
name|type
operator|!=
name|OBJ_COMMIT
condition|)
return|return
literal|"undefined"
return|;
name|c
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
expr_stmt|;
name|n
operator|=
name|c
operator|->
name|util
expr_stmt|;
if|if
condition|(
operator|!
name|n
condition|)
return|return
literal|"undefined"
return|;
if|if
condition|(
operator|!
name|n
operator|->
name|generation
condition|)
return|return
name|n
operator|->
name|tip_name
return|;
else|else
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|n
operator|->
name|tip_name
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|>
literal|2
operator|&&
operator|!
name|strcmp
argument_list|(
name|n
operator|->
name|tip_name
operator|+
name|len
operator|-
literal|2
argument_list|,
literal|"^0"
argument_list|)
condition|)
name|len
operator|-=
literal|2
expr_stmt|;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
literal|"%.*s~%d"
argument_list|,
name|len
argument_list|,
name|n
operator|->
name|tip_name
argument_list|,
name|n
operator|->
name|generation
argument_list|)
expr_stmt|;
return|return
name|buffer
return|;
block|}
block|}
end_function
begin_function
DECL|function|cmd_name_rev
name|int
name|cmd_name_rev
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
name|object_array
name|revs
init|=
block|{
literal|0
block|,
literal|0
block|,
name|NULL
block|}
decl_stmt|;
name|int
name|as_is
init|=
literal|0
decl_stmt|,
name|all
init|=
literal|0
decl_stmt|,
name|transform_stdin
init|=
literal|0
decl_stmt|;
name|struct
name|name_ref_data
name|data
init|=
block|{
literal|0
block|,
literal|0
block|,
name|NULL
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
name|usage
argument_list|(
name|name_rev_usage
argument_list|)
expr_stmt|;
for|for
control|(
operator|--
name|argc
operator|,
operator|++
name|argv
init|;
name|argc
condition|;
operator|--
name|argc
operator|,
operator|++
name|argv
control|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|object
modifier|*
name|o
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
operator|!
name|as_is
operator|&&
operator|(
operator|*
name|argv
operator|)
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|argv
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|as_is
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|argv
argument_list|,
literal|"--name-only"
argument_list|)
condition|)
block|{
name|data
operator|.
name|name_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|argv
argument_list|,
literal|"--tags"
argument_list|)
condition|)
block|{
name|data
operator|.
name|tags_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
operator|*
name|argv
argument_list|,
literal|"--refs="
argument_list|)
condition|)
block|{
name|data
operator|.
name|ref_filter
operator|=
operator|*
name|argv
operator|+
literal|7
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|argv
argument_list|,
literal|"--all"
argument_list|)
condition|)
block|{
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|die
argument_list|(
literal|"Specify either a list, or --all, not both!"
argument_list|)
expr_stmt|;
name|all
operator|=
literal|1
expr_stmt|;
name|cutoff
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|argv
argument_list|,
literal|"--stdin"
argument_list|)
condition|)
block|{
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|die
argument_list|(
literal|"Specify either a list, or --stdin, not both!"
argument_list|)
expr_stmt|;
name|transform_stdin
operator|=
literal|1
expr_stmt|;
name|cutoff
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|name_rev_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|get_sha1
argument_list|(
operator|*
name|argv
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Could not get sha1 for %s. Skipping.\n"
argument_list|,
operator|*
name|argv
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|o
operator|=
name|deref_tag
argument_list|(
name|parse_object
argument_list|(
name|sha1
argument_list|)
argument_list|,
operator|*
name|argv
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
operator|||
name|o
operator|->
name|type
operator|!=
name|OBJ_COMMIT
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Could not get commit for %s. Skipping.\n"
argument_list|,
operator|*
name|argv
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
expr_stmt|;
if|if
condition|(
name|cutoff
operator|>
name|commit
operator|->
name|date
condition|)
name|cutoff
operator|=
name|commit
operator|->
name|date
expr_stmt|;
name|add_object_array
argument_list|(
operator|(
expr|struct
name|object
operator|*
operator|)
name|commit
argument_list|,
operator|*
name|argv
argument_list|,
operator|&
name|revs
argument_list|)
expr_stmt|;
block|}
name|for_each_ref
argument_list|(
name|name_ref
argument_list|,
operator|&
name|data
argument_list|)
expr_stmt|;
if|if
condition|(
name|transform_stdin
condition|)
block|{
name|char
name|buffer
index|[
literal|2048
index|]
decl_stmt|;
name|char
modifier|*
name|p
decl_stmt|,
modifier|*
name|p_start
decl_stmt|;
while|while
condition|(
operator|!
name|feof
argument_list|(
name|stdin
argument_list|)
condition|)
block|{
name|int
name|forty
init|=
literal|0
decl_stmt|;
name|p
operator|=
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|stdin
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
break|break;
for|for
control|(
name|p_start
operator|=
name|p
init|;
operator|*
name|p
condition|;
name|p
operator|++
control|)
block|{
DECL|macro|ishex
define|#
directive|define
name|ishex
parameter_list|(
name|x
parameter_list|)
value|(isdigit((x)) || ((x)>= 'a'&& (x)<= 'f'))
if|if
condition|(
operator|!
name|ishex
argument_list|(
operator|*
name|p
argument_list|)
condition|)
name|forty
operator|=
literal|0
expr_stmt|;
elseif|else
if|if
condition|(
operator|++
name|forty
operator|==
literal|40
operator|&&
operator|!
name|ishex
argument_list|(
operator|*
operator|(
name|p
operator|+
literal|1
operator|)
argument_list|)
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|40
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
literal|"undefined"
decl_stmt|;
name|char
name|c
init|=
operator|*
operator|(
name|p
operator|+
literal|1
operator|)
decl_stmt|;
name|forty
operator|=
literal|0
expr_stmt|;
operator|*
operator|(
name|p
operator|+
literal|1
operator|)
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|p
operator|-
literal|39
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|o
condition|)
name|name
operator|=
name|get_rev_name
argument_list|(
name|o
argument_list|)
expr_stmt|;
block|}
operator|*
operator|(
name|p
operator|+
literal|1
operator|)
operator|=
name|c
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|name
argument_list|,
literal|"undefined"
argument_list|)
condition|)
continue|continue;
name|fwrite
argument_list|(
name|p_start
argument_list|,
name|p
operator|-
name|p_start
operator|+
literal|1
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|" (%s)"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|p_start
operator|=
name|p
operator|+
literal|1
expr_stmt|;
block|}
block|}
comment|/* flush */
if|if
condition|(
name|p_start
operator|!=
name|p
condition|)
name|fwrite
argument_list|(
name|p_start
argument_list|,
name|p
operator|-
name|p_start
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|all
condition|)
block|{
name|int
name|i
decl_stmt|,
name|max
decl_stmt|;
name|max
operator|=
name|get_max_object_index
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|max
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|get_indexed_object
argument_list|(
name|i
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
continue|continue;
if|if
condition|(
operator|!
name|data
operator|.
name|name_only
condition|)
name|printf
argument_list|(
literal|"%s "
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|get_rev_name
argument_list|(
name|obj
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|revs
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|data
operator|.
name|name_only
condition|)
name|printf
argument_list|(
literal|"%s "
argument_list|,
name|revs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|get_rev_name
argument_list|(
name|revs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
