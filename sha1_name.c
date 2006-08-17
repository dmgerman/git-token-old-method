begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_function
DECL|function|find_short_object_filename
specifier|static
name|int
name|find_short_object_filename
parameter_list|(
name|int
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|alternate_object_database
modifier|*
name|alt
decl_stmt|;
name|char
name|hex
index|[
literal|40
index|]
decl_stmt|;
name|int
name|found
init|=
literal|0
decl_stmt|;
specifier|static
name|struct
name|alternate_object_database
modifier|*
name|fakeent
decl_stmt|;
if|if
condition|(
operator|!
name|fakeent
condition|)
block|{
specifier|const
name|char
modifier|*
name|objdir
init|=
name|get_object_directory
argument_list|()
decl_stmt|;
name|int
name|objdir_len
init|=
name|strlen
argument_list|(
name|objdir
argument_list|)
decl_stmt|;
name|int
name|entlen
init|=
name|objdir_len
operator|+
literal|43
decl_stmt|;
name|fakeent
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|fakeent
argument_list|)
operator|+
name|entlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|fakeent
operator|->
name|base
argument_list|,
name|objdir
argument_list|,
name|objdir_len
argument_list|)
expr_stmt|;
name|fakeent
operator|->
name|name
operator|=
name|fakeent
operator|->
name|base
operator|+
name|objdir_len
operator|+
literal|1
expr_stmt|;
name|fakeent
operator|->
name|name
index|[
operator|-
literal|1
index|]
operator|=
literal|'/'
expr_stmt|;
block|}
name|fakeent
operator|->
name|next
operator|=
name|alt_odb_list
expr_stmt|;
name|sprintf
argument_list|(
name|hex
argument_list|,
literal|"%.2s"
argument_list|,
name|name
argument_list|)
expr_stmt|;
for|for
control|(
name|alt
operator|=
name|fakeent
init|;
name|alt
operator|&&
name|found
operator|<
literal|2
condition|;
name|alt
operator|=
name|alt
operator|->
name|next
control|)
block|{
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|DIR
modifier|*
name|dir
decl_stmt|;
name|sprintf
argument_list|(
name|alt
operator|->
name|name
argument_list|,
literal|"%.2s/"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|dir
operator|=
name|opendir
argument_list|(
name|alt
operator|->
name|base
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
continue|continue;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
operator|!=
literal|38
condition|)
continue|continue;
if|if
condition|(
name|memcmp
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
name|name
operator|+
literal|2
argument_list|,
name|len
operator|-
literal|2
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|found
condition|)
block|{
name|memcpy
argument_list|(
name|hex
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
expr_stmt|;
name|found
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|memcmp
argument_list|(
name|hex
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
condition|)
block|{
name|found
operator|=
literal|2
expr_stmt|;
break|break;
block|}
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|found
operator|==
literal|1
condition|)
return|return
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|sha1
argument_list|)
operator|==
literal|0
return|;
return|return
name|found
return|;
block|}
end_function
begin_function
DECL|function|match_sha
specifier|static
name|int
name|match_sha
parameter_list|(
name|unsigned
name|len
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|a
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|b
parameter_list|)
block|{
do|do
block|{
if|if
condition|(
operator|*
name|a
operator|!=
operator|*
name|b
condition|)
return|return
literal|0
return|;
name|a
operator|++
expr_stmt|;
name|b
operator|++
expr_stmt|;
name|len
operator|-=
literal|2
expr_stmt|;
block|}
do|while
condition|(
name|len
operator|>
literal|1
condition|)
do|;
if|if
condition|(
name|len
condition|)
if|if
condition|(
operator|(
operator|*
name|a
operator|^
operator|*
name|b
operator|)
operator|&
literal|0xf0
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|find_short_packed_object
specifier|static
name|int
name|find_short_packed_object
parameter_list|(
name|int
name|len
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|match
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
name|unsigned
name|char
name|found_sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|found
init|=
literal|0
decl_stmt|;
name|prepare_packed_git
argument_list|()
expr_stmt|;
for|for
control|(
name|p
operator|=
name|packed_git
init|;
name|p
operator|&&
name|found
operator|<
literal|2
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
name|unsigned
name|num
init|=
name|num_packed_objects
argument_list|(
name|p
argument_list|)
decl_stmt|;
name|unsigned
name|first
init|=
literal|0
decl_stmt|,
name|last
init|=
name|num
decl_stmt|;
while|while
condition|(
name|first
operator|<
name|last
condition|)
block|{
name|unsigned
name|mid
init|=
operator|(
name|first
operator|+
name|last
operator|)
operator|/
literal|2
decl_stmt|;
name|unsigned
name|char
name|now
index|[
literal|20
index|]
decl_stmt|;
name|int
name|cmp
decl_stmt|;
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|mid
argument_list|,
name|now
argument_list|)
expr_stmt|;
name|cmp
operator|=
name|hashcmp
argument_list|(
name|match
argument_list|,
name|now
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
block|{
name|first
operator|=
name|mid
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
block|{
name|first
operator|=
name|mid
operator|+
literal|1
expr_stmt|;
continue|continue;
block|}
name|last
operator|=
name|mid
expr_stmt|;
block|}
if|if
condition|(
name|first
operator|<
name|num
condition|)
block|{
name|unsigned
name|char
name|now
index|[
literal|20
index|]
decl_stmt|,
name|next
index|[
literal|20
index|]
decl_stmt|;
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|first
argument_list|,
name|now
argument_list|)
expr_stmt|;
if|if
condition|(
name|match_sha
argument_list|(
name|len
argument_list|,
name|match
argument_list|,
name|now
argument_list|)
condition|)
block|{
if|if
condition|(
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|first
operator|+
literal|1
argument_list|,
name|next
argument_list|)
operator|||
operator|!
name|match_sha
argument_list|(
name|len
argument_list|,
name|match
argument_list|,
name|next
argument_list|)
condition|)
block|{
comment|/* unique within this pack */
if|if
condition|(
operator|!
name|found
condition|)
block|{
name|memcpy
argument_list|(
name|found_sha1
argument_list|,
name|now
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|found
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|hashcmp
argument_list|(
name|found_sha1
argument_list|,
name|now
argument_list|)
condition|)
block|{
name|found
operator|=
literal|2
expr_stmt|;
break|break;
block|}
block|}
else|else
block|{
comment|/* not even unique within this pack */
name|found
operator|=
literal|2
expr_stmt|;
break|break;
block|}
block|}
block|}
block|}
if|if
condition|(
name|found
operator|==
literal|1
condition|)
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|found_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|found
return|;
block|}
end_function
begin_define
DECL|macro|SHORT_NAME_NOT_FOUND
define|#
directive|define
name|SHORT_NAME_NOT_FOUND
value|(-1)
end_define
begin_define
DECL|macro|SHORT_NAME_AMBIGUOUS
define|#
directive|define
name|SHORT_NAME_AMBIGUOUS
value|(-2)
end_define
begin_function
DECL|function|find_unique_short_object
specifier|static
name|int
name|find_unique_short_object
parameter_list|(
name|int
name|len
parameter_list|,
name|char
modifier|*
name|canonical
parameter_list|,
name|unsigned
name|char
modifier|*
name|res
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|has_unpacked
decl_stmt|,
name|has_packed
decl_stmt|;
name|unsigned
name|char
name|unpacked_sha1
index|[
literal|20
index|]
decl_stmt|,
name|packed_sha1
index|[
literal|20
index|]
decl_stmt|;
name|has_unpacked
operator|=
name|find_short_object_filename
argument_list|(
name|len
argument_list|,
name|canonical
argument_list|,
name|unpacked_sha1
argument_list|)
expr_stmt|;
name|has_packed
operator|=
name|find_short_packed_object
argument_list|(
name|len
argument_list|,
name|res
argument_list|,
name|packed_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|has_unpacked
operator|&&
operator|!
name|has_packed
condition|)
return|return
name|SHORT_NAME_NOT_FOUND
return|;
if|if
condition|(
literal|1
operator|<
name|has_unpacked
operator|||
literal|1
operator|<
name|has_packed
condition|)
return|return
name|SHORT_NAME_AMBIGUOUS
return|;
if|if
condition|(
name|has_unpacked
operator|!=
name|has_packed
condition|)
block|{
name|memcpy
argument_list|(
name|sha1
argument_list|,
operator|(
name|has_packed
condition|?
name|packed_sha1
else|:
name|unpacked_sha1
operator|)
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* Both have unique ones -- do they match? */
if|if
condition|(
name|hashcmp
argument_list|(
name|packed_sha1
argument_list|,
name|unpacked_sha1
argument_list|)
condition|)
return|return
name|SHORT_NAME_AMBIGUOUS
return|;
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|packed_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_short_sha1
specifier|static
name|int
name|get_short_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|quietly
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|status
decl_stmt|;
name|char
name|canonical
index|[
literal|40
index|]
decl_stmt|;
name|unsigned
name|char
name|res
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|len
operator|<
name|MINIMUM_ABBREV
condition|)
return|return
operator|-
literal|1
return|;
name|memset
argument_list|(
name|res
argument_list|,
literal|0
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|canonical
argument_list|,
literal|'x'
argument_list|,
literal|40
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|len
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|char
name|c
init|=
name|name
index|[
name|i
index|]
decl_stmt|;
name|unsigned
name|char
name|val
decl_stmt|;
if|if
condition|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
condition|)
name|val
operator|=
name|c
operator|-
literal|'0'
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'f'
condition|)
name|val
operator|=
name|c
operator|-
literal|'a'
operator|+
literal|10
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'F'
condition|)
block|{
name|val
operator|=
name|c
operator|-
literal|'A'
operator|+
literal|10
expr_stmt|;
name|c
operator|-=
literal|'A'
operator|-
literal|'a'
expr_stmt|;
block|}
else|else
return|return
operator|-
literal|1
return|;
name|canonical
index|[
name|i
index|]
operator|=
name|c
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|i
operator|&
literal|1
operator|)
condition|)
name|val
operator|<<=
literal|4
expr_stmt|;
name|res
index|[
name|i
operator|>>
literal|1
index|]
operator||=
name|val
expr_stmt|;
block|}
name|status
operator|=
name|find_unique_short_object
argument_list|(
name|i
argument_list|,
name|canonical
argument_list|,
name|res
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|quietly
operator|&&
operator|(
name|status
operator|==
name|SHORT_NAME_AMBIGUOUS
operator|)
condition|)
return|return
name|error
argument_list|(
literal|"short SHA1 %.*s is ambiguous."
argument_list|,
name|len
argument_list|,
name|canonical
argument_list|)
return|;
return|return
name|status
return|;
block|}
end_function
begin_function
DECL|function|find_unique_abbrev
specifier|const
name|char
modifier|*
name|find_unique_abbrev
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|int
name|status
decl_stmt|,
name|is_null
decl_stmt|;
specifier|static
name|char
name|hex
index|[
literal|41
index|]
decl_stmt|;
name|is_null
operator|=
name|is_null_sha1
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
literal|40
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|==
literal|40
operator|||
operator|!
name|len
condition|)
return|return
name|hex
return|;
while|while
condition|(
name|len
operator|<
literal|40
condition|)
block|{
name|unsigned
name|char
name|sha1_ret
index|[
literal|20
index|]
decl_stmt|;
name|status
operator|=
name|get_short_sha1
argument_list|(
name|hex
argument_list|,
name|len
argument_list|,
name|sha1_ret
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|status
operator|||
operator|(
name|is_null
operator|&&
name|status
operator|!=
name|SHORT_NAME_AMBIGUOUS
operator|)
condition|)
block|{
name|hex
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|hex
return|;
block|}
if|if
condition|(
name|status
operator|!=
name|SHORT_NAME_AMBIGUOUS
condition|)
return|return
name|NULL
return|;
name|len
operator|++
expr_stmt|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|ambiguous_path
specifier|static
name|int
name|ambiguous_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|int
name|slash
init|=
literal|1
decl_stmt|;
name|int
name|cnt
decl_stmt|;
for|for
control|(
name|cnt
operator|=
literal|0
init|;
name|cnt
operator|<
name|len
condition|;
name|cnt
operator|++
control|)
block|{
switch|switch
condition|(
operator|*
name|path
operator|++
condition|)
block|{
case|case
literal|'\0'
case|:
break|break;
case|case
literal|'/'
case|:
if|if
condition|(
name|slash
condition|)
break|break;
name|slash
operator|=
literal|1
expr_stmt|;
continue|continue;
case|case
literal|'.'
case|:
continue|continue;
default|default:
name|slash
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
return|return
name|slash
return|;
block|}
end_function
begin_function
DECL|function|get_sha1_basic
specifier|static
name|int
name|get_sha1_basic
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
specifier|const
name|char
modifier|*
name|fmt
index|[]
init|=
block|{
literal|"%.*s"
block|,
literal|"refs/%.*s"
block|,
literal|"refs/tags/%.*s"
block|,
literal|"refs/heads/%.*s"
block|,
literal|"refs/remotes/%.*s"
block|,
literal|"refs/remotes/%.*s/HEAD"
block|,
name|NULL
block|}
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|warning
init|=
literal|"warning: refname '%.*s' is ambiguous.\n"
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|,
modifier|*
name|pathname
decl_stmt|;
name|char
modifier|*
name|real_path
init|=
name|NULL
decl_stmt|;
name|int
name|refs_found
init|=
literal|0
decl_stmt|,
name|am
decl_stmt|;
name|unsigned
name|long
name|at_time
init|=
operator|(
name|unsigned
name|long
operator|)
operator|-
literal|1
decl_stmt|;
name|unsigned
name|char
modifier|*
name|this_result
decl_stmt|;
name|unsigned
name|char
name|sha1_from_ref
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|len
operator|==
literal|40
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|str
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* At a given period of time? "@{2 hours ago}" */
for|for
control|(
name|am
operator|=
literal|1
init|;
name|am
operator|<
name|len
operator|-
literal|1
condition|;
name|am
operator|++
control|)
block|{
if|if
condition|(
name|str
index|[
name|am
index|]
operator|==
literal|'@'
operator|&&
name|str
index|[
name|am
operator|+
literal|1
index|]
operator|==
literal|'{'
operator|&&
name|str
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'}'
condition|)
block|{
name|int
name|date_len
init|=
name|len
operator|-
name|am
operator|-
literal|3
decl_stmt|;
name|char
modifier|*
name|date_spec
init|=
name|xmalloc
argument_list|(
name|date_len
operator|+
literal|1
argument_list|)
decl_stmt|;
name|strlcpy
argument_list|(
name|date_spec
argument_list|,
name|str
operator|+
name|am
operator|+
literal|2
argument_list|,
name|date_len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|at_time
operator|=
name|approxidate
argument_list|(
name|date_spec
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|date_spec
argument_list|)
expr_stmt|;
name|len
operator|=
name|am
expr_stmt|;
break|break;
block|}
block|}
comment|/* Accept only unambiguous ref paths. */
if|if
condition|(
name|ambiguous_path
argument_list|(
name|str
argument_list|,
name|len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|p
operator|=
name|fmt
init|;
operator|*
name|p
condition|;
name|p
operator|++
control|)
block|{
name|this_result
operator|=
name|refs_found
condition|?
name|sha1_from_ref
else|:
name|sha1
expr_stmt|;
name|pathname
operator|=
name|resolve_ref
argument_list|(
name|git_path
argument_list|(
operator|*
name|p
argument_list|,
name|len
argument_list|,
name|str
argument_list|)
argument_list|,
name|this_result
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathname
condition|)
block|{
if|if
condition|(
operator|!
name|refs_found
operator|++
condition|)
name|real_path
operator|=
name|strdup
argument_list|(
name|pathname
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|warn_ambiguous_refs
condition|)
break|break;
block|}
block|}
if|if
condition|(
operator|!
name|refs_found
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|warn_ambiguous_refs
operator|&&
name|refs_found
operator|>
literal|1
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|warning
argument_list|,
name|len
argument_list|,
name|str
argument_list|)
expr_stmt|;
if|if
condition|(
name|at_time
operator|!=
operator|(
name|unsigned
name|long
operator|)
operator|-
literal|1
condition|)
block|{
name|read_ref_at
argument_list|(
name|real_path
operator|+
name|strlen
argument_list|(
name|git_path
argument_list|(
literal|"."
argument_list|)
argument_list|)
operator|-
literal|1
argument_list|,
name|at_time
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|real_path
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function_decl
specifier|static
name|int
name|get_sha1_1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|get_parent
specifier|static
name|int
name|get_parent
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|int
name|idx
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|ret
init|=
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|idx
condition|)
block|{
name|memcpy
argument_list|(
name|result
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|p
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
if|if
condition|(
operator|!
operator|--
name|idx
condition|)
block|{
name|memcpy
argument_list|(
name|result
argument_list|,
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|get_nth_ancestor
specifier|static
name|int
name|get_nth_ancestor
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|int
name|generation
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|ret
init|=
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
while|while
condition|(
name|generation
operator|--
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|commit
operator|||
name|parse_commit
argument_list|(
name|commit
argument_list|)
operator|||
operator|!
name|commit
operator|->
name|parents
condition|)
return|return
operator|-
literal|1
return|;
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|commit
operator|->
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|result
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|peel_onion
specifier|static
name|int
name|peel_onion
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|char
name|outer
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|sp
decl_stmt|;
name|unsigned
name|int
name|expected_type
init|=
literal|0
decl_stmt|;
name|struct
name|object
modifier|*
name|o
decl_stmt|;
comment|/* 	 * "ref^{type}" dereferences ref repeatedly until you cannot 	 * dereference anymore, or you get an object of given type, 	 * whichever comes first.  "ref^{}" means just dereference 	 * tags until you get a non-tag.  "ref^0" is a shorthand for 	 * "ref^{commit}".  "commit^{tree}" could be used to find the 	 * top-level tree of the given commit. 	 */
if|if
condition|(
name|len
operator|<
literal|4
operator|||
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'}'
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|sp
operator|=
name|name
operator|+
name|len
operator|-
literal|1
init|;
name|name
operator|<=
name|sp
condition|;
name|sp
operator|--
control|)
block|{
name|int
name|ch
init|=
operator|*
name|sp
decl_stmt|;
if|if
condition|(
name|ch
operator|==
literal|'{'
operator|&&
name|name
operator|<
name|sp
operator|&&
name|sp
index|[
operator|-
literal|1
index|]
operator|==
literal|'^'
condition|)
break|break;
block|}
if|if
condition|(
name|sp
operator|<=
name|name
condition|)
return|return
operator|-
literal|1
return|;
name|sp
operator|++
expr_stmt|;
comment|/* beginning of type name, or closing brace for empty */
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|commit_type
argument_list|,
name|sp
argument_list|,
literal|6
argument_list|)
operator|&&
name|sp
index|[
literal|6
index|]
operator|==
literal|'}'
condition|)
name|expected_type
operator|=
name|OBJ_COMMIT
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|tree_type
argument_list|,
name|sp
argument_list|,
literal|4
argument_list|)
operator|&&
name|sp
index|[
literal|4
index|]
operator|==
literal|'}'
condition|)
name|expected_type
operator|=
name|OBJ_TREE
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|blob_type
argument_list|,
name|sp
argument_list|,
literal|4
argument_list|)
operator|&&
name|sp
index|[
literal|4
index|]
operator|==
literal|'}'
condition|)
name|expected_type
operator|=
name|OBJ_BLOB
expr_stmt|;
elseif|else
if|if
condition|(
name|sp
index|[
literal|0
index|]
operator|==
literal|'}'
condition|)
name|expected_type
operator|=
name|OBJ_NONE
expr_stmt|;
else|else
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|sp
operator|-
name|name
operator|-
literal|2
argument_list|,
name|outer
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|o
operator|=
name|parse_object
argument_list|(
name|outer
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|expected_type
condition|)
block|{
name|o
operator|=
name|deref_tag
argument_list|(
name|o
argument_list|,
name|name
argument_list|,
name|sp
operator|-
name|name
operator|-
literal|2
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
operator|||
operator|(
operator|!
name|o
operator|->
name|parsed
operator|&&
operator|!
name|parse_object
argument_list|(
name|o
operator|->
name|sha1
argument_list|)
operator|)
condition|)
return|return
operator|-
literal|1
return|;
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|o
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* At this point, the syntax look correct, so 		 * if we do not get the needed object, we should 		 * barf. 		 */
while|while
condition|(
literal|1
condition|)
block|{
if|if
condition|(
operator|!
name|o
operator|||
operator|(
operator|!
name|o
operator|->
name|parsed
operator|&&
operator|!
name|parse_object
argument_list|(
name|o
operator|->
name|sha1
argument_list|)
operator|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|expected_type
condition|)
block|{
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|o
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
name|o
operator|=
operator|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|o
operator|)
operator|->
name|tagged
expr_stmt|;
elseif|else
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
name|o
operator|=
operator|&
operator|(
operator|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
operator|)
operator|->
name|tree
operator|->
name|object
operator|)
expr_stmt|;
else|else
return|return
name|error
argument_list|(
literal|"%.*s: expected %s type, but the object dereferences to %s type"
argument_list|,
name|len
argument_list|,
name|name
argument_list|,
name|typename
argument_list|(
name|expected_type
argument_list|)
argument_list|,
name|typename
argument_list|(
name|o
operator|->
name|type
argument_list|)
argument_list|)
return|;
if|if
condition|(
operator|!
name|o
operator|->
name|parsed
condition|)
name|parse_object
argument_list|(
name|o
operator|->
name|sha1
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
DECL|function|get_sha1_1
specifier|static
name|int
name|get_sha1_1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|ret
decl_stmt|,
name|has_suffix
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
comment|/* "name~3" is "name^^^", 	 * "name~" and "name~0" are name -- not "name^0"! 	 * "name^" is not "name^0"; it is "name^1". 	 */
name|has_suffix
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|name
operator|+
name|len
operator|-
literal|1
init|;
name|name
operator|<=
name|cp
condition|;
name|cp
operator|--
control|)
block|{
name|int
name|ch
init|=
operator|*
name|cp
decl_stmt|;
if|if
condition|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'9'
condition|)
continue|continue;
if|if
condition|(
name|ch
operator|==
literal|'~'
operator|||
name|ch
operator|==
literal|'^'
condition|)
name|has_suffix
operator|=
name|ch
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|has_suffix
condition|)
block|{
name|int
name|num
init|=
literal|0
decl_stmt|;
name|int
name|len1
init|=
name|cp
operator|-
name|name
decl_stmt|;
name|cp
operator|++
expr_stmt|;
while|while
condition|(
name|cp
operator|<
name|name
operator|+
name|len
condition|)
name|num
operator|=
name|num
operator|*
literal|10
operator|+
operator|*
name|cp
operator|++
operator|-
literal|'0'
expr_stmt|;
if|if
condition|(
name|has_suffix
operator|==
literal|'^'
condition|)
block|{
if|if
condition|(
operator|!
name|num
operator|&&
name|len1
operator|==
name|len
operator|-
literal|1
condition|)
name|num
operator|=
literal|1
expr_stmt|;
return|return
name|get_parent
argument_list|(
name|name
argument_list|,
name|len1
argument_list|,
name|sha1
argument_list|,
name|num
argument_list|)
return|;
block|}
comment|/* else if (has_suffix == '~') -- goes without saying */
return|return
name|get_nth_ancestor
argument_list|(
name|name
argument_list|,
name|len1
argument_list|,
name|sha1
argument_list|,
name|num
argument_list|)
return|;
block|}
name|ret
operator|=
name|peel_onion
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
return|return
literal|0
return|;
name|ret
operator|=
name|get_sha1_basic
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
return|return
literal|0
return|;
return|return
name|get_short_sha1
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * This is like "get_sha1_basic()", except it allows "sha1 expressions",  * notably "xyz^" for "parent of xyz"  */
end_comment
begin_function
DECL|function|get_sha1
name|int
name|get_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|ret
decl_stmt|,
name|bracket_depth
decl_stmt|;
name|unsigned
name|unused
decl_stmt|;
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|prepare_alt_odb
argument_list|()
expr_stmt|;
name|ret
operator|=
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
return|return
name|ret
return|;
comment|/* sha1:path --> object name of path in ent sha1 	 * :path -> object name of path in index 	 * :[0-3]:path -> object name of path in index at stage 	 */
if|if
condition|(
name|name
index|[
literal|0
index|]
operator|==
literal|':'
condition|)
block|{
name|int
name|stage
init|=
literal|0
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|int
name|pos
decl_stmt|;
if|if
condition|(
name|namelen
operator|<
literal|3
operator|||
name|name
index|[
literal|2
index|]
operator|!=
literal|':'
operator|||
name|name
index|[
literal|1
index|]
operator|<
literal|'0'
operator|||
literal|'3'
operator|<
name|name
index|[
literal|1
index|]
condition|)
name|cp
operator|=
name|name
operator|+
literal|1
expr_stmt|;
else|else
block|{
name|stage
operator|=
name|name
index|[
literal|1
index|]
operator|-
literal|'0'
expr_stmt|;
name|cp
operator|=
name|name
operator|+
literal|3
expr_stmt|;
block|}
name|namelen
operator|=
name|namelen
operator|-
operator|(
name|cp
operator|-
name|name
operator|)
expr_stmt|;
if|if
condition|(
operator|!
name|active_cache
condition|)
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_nr
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|cp
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|!=
name|namelen
operator|||
name|memcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|cp
argument_list|,
name|namelen
argument_list|)
condition|)
break|break;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|==
name|stage
condition|)
block|{
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|pos
operator|++
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
for|for
control|(
name|cp
operator|=
name|name
operator|,
name|bracket_depth
operator|=
literal|0
init|;
operator|*
name|cp
condition|;
name|cp
operator|++
control|)
block|{
if|if
condition|(
operator|*
name|cp
operator|==
literal|'{'
condition|)
name|bracket_depth
operator|++
expr_stmt|;
elseif|else
if|if
condition|(
name|bracket_depth
operator|&&
operator|*
name|cp
operator|==
literal|'}'
condition|)
name|bracket_depth
operator|--
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|bracket_depth
operator|&&
operator|*
name|cp
operator|==
literal|':'
condition|)
break|break;
block|}
if|if
condition|(
operator|*
name|cp
operator|==
literal|':'
condition|)
block|{
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|cp
operator|-
name|name
argument_list|,
name|tree_sha1
argument_list|)
condition|)
return|return
name|get_tree_entry
argument_list|(
name|tree_sha1
argument_list|,
name|cp
operator|+
literal|1
argument_list|,
name|sha1
argument_list|,
operator|&
name|unused
argument_list|)
return|;
block|}
return|return
name|ret
return|;
block|}
end_function
end_unit
