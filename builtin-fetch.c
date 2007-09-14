begin_unit
begin_comment
comment|/*  * "git fetch"  */
end_comment
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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"path-list.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_include
include|#
directive|include
file|"transport.h"
end_include
begin_decl_stmt
DECL|variable|fetch_usage
specifier|static
specifier|const
name|char
name|fetch_usage
index|[]
init|=
literal|"git-fetch [-a | --append] [--upload-pack<upload-pack>] [-f | --force] [--no-tags] [-t | --tags] [-k | --keep] [-u | --update-head-ok] [--depth<depth>] [-v | --verbose] [<repository><refspec>...]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|append
DECL|variable|force
DECL|variable|tags
DECL|variable|no_tags
DECL|variable|update_head_ok
DECL|variable|verbose
DECL|variable|quiet
specifier|static
name|int
name|append
decl_stmt|,
name|force
decl_stmt|,
name|tags
decl_stmt|,
name|no_tags
decl_stmt|,
name|update_head_ok
decl_stmt|,
name|verbose
decl_stmt|,
name|quiet
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|default_rla
specifier|static
name|char
modifier|*
name|default_rla
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|find_merge_config
specifier|static
name|void
name|find_merge_config
parameter_list|(
name|struct
name|ref
modifier|*
name|ref_map
parameter_list|,
name|struct
name|remote
modifier|*
name|remote
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|rm
init|=
name|ref_map
decl_stmt|;
name|struct
name|branch
modifier|*
name|branch
init|=
name|branch_get
argument_list|(
name|NULL
argument_list|)
decl_stmt|;
for|for
control|(
name|rm
operator|=
name|ref_map
init|;
name|rm
condition|;
name|rm
operator|=
name|rm
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|branch_has_merge_config
argument_list|(
name|branch
argument_list|)
condition|)
block|{
if|if
condition|(
name|remote
operator|&&
name|remote
operator|->
name|fetch
operator|&&
operator|!
name|strcmp
argument_list|(
name|remote
operator|->
name|fetch
index|[
literal|0
index|]
operator|.
name|src
argument_list|,
name|rm
operator|->
name|name
argument_list|)
condition|)
name|rm
operator|->
name|merge
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|branch_merges
argument_list|(
name|branch
argument_list|,
name|rm
operator|->
name|name
argument_list|)
condition|)
name|rm
operator|->
name|merge
operator|=
literal|1
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|get_ref_map
specifier|static
name|struct
name|ref
modifier|*
name|get_ref_map
parameter_list|(
name|struct
name|transport
modifier|*
name|transport
parameter_list|,
name|struct
name|refspec
modifier|*
name|refs
parameter_list|,
name|int
name|ref_count
parameter_list|,
name|int
name|tags
parameter_list|,
name|int
modifier|*
name|autotags
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|ref
modifier|*
name|rm
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref_map
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
modifier|*
name|tail
init|=
operator|&
name|ref_map
decl_stmt|;
name|struct
name|ref
modifier|*
name|remote_refs
init|=
name|transport_get_remote_refs
argument_list|(
name|transport
argument_list|)
decl_stmt|;
if|if
condition|(
name|ref_count
operator|||
name|tags
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ref_count
condition|;
name|i
operator|++
control|)
block|{
name|get_fetch_map
argument_list|(
name|remote_refs
argument_list|,
operator|&
name|refs
index|[
name|i
index|]
argument_list|,
operator|&
name|tail
argument_list|)
expr_stmt|;
if|if
condition|(
name|refs
index|[
name|i
index|]
operator|.
name|dst
operator|&&
name|refs
index|[
name|i
index|]
operator|.
name|dst
index|[
literal|0
index|]
condition|)
operator|*
name|autotags
operator|=
literal|1
expr_stmt|;
block|}
comment|/* Merge everything on the command line, but not --tags */
for|for
control|(
name|rm
operator|=
name|ref_map
init|;
name|rm
condition|;
name|rm
operator|=
name|rm
operator|->
name|next
control|)
name|rm
operator|->
name|merge
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|tags
condition|)
block|{
name|struct
name|refspec
name|refspec
decl_stmt|;
name|refspec
operator|.
name|src
operator|=
literal|"refs/tags/"
expr_stmt|;
name|refspec
operator|.
name|dst
operator|=
literal|"refs/tags/"
expr_stmt|;
name|refspec
operator|.
name|pattern
operator|=
literal|1
expr_stmt|;
name|refspec
operator|.
name|force
operator|=
literal|0
expr_stmt|;
name|get_fetch_map
argument_list|(
name|remote_refs
argument_list|,
operator|&
name|refspec
argument_list|,
operator|&
name|tail
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
comment|/* Use the defaults */
name|struct
name|remote
modifier|*
name|remote
init|=
name|transport
operator|->
name|remote
decl_stmt|;
if|if
condition|(
name|remote
operator|->
name|fetch_refspec_nr
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|remote
operator|->
name|fetch_refspec_nr
condition|;
name|i
operator|++
control|)
block|{
name|get_fetch_map
argument_list|(
name|remote_refs
argument_list|,
operator|&
name|remote
operator|->
name|fetch
index|[
name|i
index|]
argument_list|,
operator|&
name|tail
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
operator|->
name|fetch
index|[
name|i
index|]
operator|.
name|dst
operator|&&
name|remote
operator|->
name|fetch
index|[
name|i
index|]
operator|.
name|dst
index|[
literal|0
index|]
condition|)
operator|*
name|autotags
operator|=
literal|1
expr_stmt|;
block|}
name|find_merge_config
argument_list|(
name|ref_map
argument_list|,
name|remote
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|ref_map
operator|=
name|get_remote_ref
argument_list|(
name|remote_refs
argument_list|,
literal|"HEAD"
argument_list|)
expr_stmt|;
name|ref_map
operator|->
name|merge
operator|=
literal|1
expr_stmt|;
block|}
block|}
return|return
name|ref_map
return|;
block|}
end_function
begin_function
DECL|function|show_new
specifier|static
name|void
name|show_new
parameter_list|(
name|enum
name|object_type
name|type
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1_new
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"  %s: %s\n"
argument_list|,
name|typename
argument_list|(
name|type
argument_list|)
argument_list|,
name|find_unique_abbrev
argument_list|(
name|sha1_new
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|s_update_ref
specifier|static
name|int
name|s_update_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|action
parameter_list|,
name|struct
name|ref
modifier|*
name|ref
parameter_list|,
name|int
name|check_old
parameter_list|)
block|{
name|char
name|msg
index|[
literal|1024
index|]
decl_stmt|;
name|char
modifier|*
name|rla
init|=
name|getenv
argument_list|(
literal|"GIT_REFLOG_ACTION"
argument_list|)
decl_stmt|;
specifier|static
name|struct
name|ref_lock
modifier|*
name|lock
decl_stmt|;
if|if
condition|(
operator|!
name|rla
condition|)
name|rla
operator|=
name|default_rla
expr_stmt|;
name|snprintf
argument_list|(
name|msg
argument_list|,
sizeof|sizeof
argument_list|(
name|msg
argument_list|)
argument_list|,
literal|"%s: %s"
argument_list|,
name|rla
argument_list|,
name|action
argument_list|)
expr_stmt|;
name|lock
operator|=
name|lock_any_ref_for_update
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|check_old
condition|?
name|ref
operator|->
name|old_sha1
else|:
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|lock
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|write_ref_sha1
argument_list|(
name|lock
argument_list|,
name|ref
operator|->
name|new_sha1
argument_list|,
name|msg
argument_list|)
operator|<
literal|0
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|update_local_ref
specifier|static
name|int
name|update_local_ref
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|,
specifier|const
name|char
modifier|*
name|note
parameter_list|,
name|int
name|verbose
parameter_list|)
block|{
name|char
name|oldh
index|[
literal|41
index|]
decl_stmt|,
name|newh
index|[
literal|41
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|current
init|=
name|NULL
decl_stmt|,
modifier|*
name|updated
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|struct
name|branch
modifier|*
name|current_branch
init|=
name|branch_get
argument_list|(
name|NULL
argument_list|)
decl_stmt|;
name|type
operator|=
name|sha1_object_info
argument_list|(
name|ref
operator|->
name|new_sha1
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|type
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"object %s not found"
argument_list|,
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|new_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|ref
operator|->
name|name
condition|)
block|{
comment|/* Not storing */
if|if
condition|(
name|verbose
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* fetched %s\n"
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|show_new
argument_list|(
name|type
argument_list|,
name|ref
operator|->
name|new_sha1
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|hashcmp
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|,
name|ref
operator|->
name|new_sha1
argument_list|)
condition|)
block|{
if|if
condition|(
name|verbose
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* %s: same as %s\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|show_new
argument_list|(
name|type
argument_list|,
name|ref
operator|->
name|new_sha1
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|current_branch
operator|->
name|name
argument_list|)
operator|&&
operator|!
operator|(
name|update_head_ok
operator|||
name|is_bare_repository
argument_list|()
operator|)
operator|&&
operator|!
name|is_null_sha1
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
condition|)
block|{
comment|/* 		 * If this is the head, and it's not okay to update 		 * the head, and the old value of the head isn't empty... 		 */
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" * %s: Cannot fetch into the current branch.\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|is_null_sha1
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/tags/"
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* %s: updating with %s\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|show_new
argument_list|(
name|type
argument_list|,
name|ref
operator|->
name|new_sha1
argument_list|)
expr_stmt|;
return|return
name|s_update_ref
argument_list|(
literal|"updating tag"
argument_list|,
name|ref
argument_list|,
literal|0
argument_list|)
return|;
block|}
name|current
operator|=
name|lookup_commit_reference
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
name|updated
operator|=
name|lookup_commit_reference
argument_list|(
name|ref
operator|->
name|new_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|current
operator|||
operator|!
name|updated
condition|)
block|{
name|char
modifier|*
name|msg
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/tags/"
argument_list|,
literal|10
argument_list|)
condition|)
name|msg
operator|=
literal|"storing tag"
expr_stmt|;
else|else
name|msg
operator|=
literal|"storing head"
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* %s: storing %s\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|show_new
argument_list|(
name|type
argument_list|,
name|ref
operator|->
name|new_sha1
argument_list|)
expr_stmt|;
return|return
name|s_update_ref
argument_list|(
name|msg
argument_list|,
name|ref
argument_list|,
literal|0
argument_list|)
return|;
block|}
name|strcpy
argument_list|(
name|oldh
argument_list|,
name|find_unique_abbrev
argument_list|(
name|current
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|newh
argument_list|,
name|find_unique_abbrev
argument_list|(
name|ref
operator|->
name|new_sha1
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|in_merge_bases
argument_list|(
name|current
argument_list|,
operator|&
name|updated
argument_list|,
literal|1
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* %s: fast forward to %s\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"  old..new: %s..%s\n"
argument_list|,
name|oldh
argument_list|,
name|newh
argument_list|)
expr_stmt|;
return|return
name|s_update_ref
argument_list|(
literal|"fast forward"
argument_list|,
name|ref
argument_list|,
literal|1
argument_list|)
return|;
block|}
if|if
condition|(
operator|!
name|force
operator|&&
operator|!
name|ref
operator|->
name|force
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* %s: not updating to non-fast forward %s\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"  old...new: %s...%s\n"
argument_list|,
name|oldh
argument_list|,
name|newh
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"* %s: forcing update to non-fast forward %s\n"
argument_list|,
name|ref
operator|->
name|name
argument_list|,
name|note
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"  old...new: %s...%s\n"
argument_list|,
name|oldh
argument_list|,
name|newh
argument_list|)
expr_stmt|;
return|return
name|s_update_ref
argument_list|(
literal|"forced-update"
argument_list|,
name|ref
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|store_updated_refs
specifier|static
name|void
name|store_updated_refs
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|struct
name|ref
modifier|*
name|ref_map
parameter_list|)
block|{
name|FILE
modifier|*
name|fp
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|int
name|url_len
decl_stmt|,
name|i
decl_stmt|,
name|note_len
decl_stmt|;
name|char
name|note
index|[
literal|1024
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|what
decl_stmt|,
modifier|*
name|kind
decl_stmt|;
name|struct
name|ref
modifier|*
name|rm
decl_stmt|;
name|fp
operator|=
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"FETCH_HEAD"
argument_list|)
argument_list|,
literal|"a"
argument_list|)
expr_stmt|;
for|for
control|(
name|rm
operator|=
name|ref_map
init|;
name|rm
condition|;
name|rm
operator|=
name|rm
operator|->
name|next
control|)
block|{
name|struct
name|ref
modifier|*
name|ref
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|rm
operator|->
name|peer_ref
condition|)
block|{
name|ref
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ref
argument_list|)
operator|+
name|strlen
argument_list|(
name|rm
operator|->
name|peer_ref
operator|->
name|name
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|rm
operator|->
name|peer_ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|,
name|rm
operator|->
name|peer_ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|ref
operator|->
name|new_sha1
argument_list|,
name|rm
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
name|ref
operator|->
name|force
operator|=
name|rm
operator|->
name|peer_ref
operator|->
name|force
expr_stmt|;
block|}
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|rm
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
name|rm
operator|->
name|merge
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|rm
operator|->
name|name
argument_list|,
literal|"HEAD"
argument_list|)
condition|)
block|{
name|kind
operator|=
literal|""
expr_stmt|;
name|what
operator|=
literal|""
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|rm
operator|->
name|name
argument_list|,
literal|"refs/heads/"
argument_list|)
condition|)
block|{
name|kind
operator|=
literal|"branch"
expr_stmt|;
name|what
operator|=
name|rm
operator|->
name|name
operator|+
literal|11
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|rm
operator|->
name|name
argument_list|,
literal|"refs/tags/"
argument_list|)
condition|)
block|{
name|kind
operator|=
literal|"tag"
expr_stmt|;
name|what
operator|=
name|rm
operator|->
name|name
operator|+
literal|10
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|rm
operator|->
name|name
argument_list|,
literal|"refs/remotes/"
argument_list|)
condition|)
block|{
name|kind
operator|=
literal|"remote branch"
expr_stmt|;
name|what
operator|=
name|rm
operator|->
name|name
operator|+
literal|13
expr_stmt|;
block|}
else|else
block|{
name|kind
operator|=
literal|""
expr_stmt|;
name|what
operator|=
name|rm
operator|->
name|name
expr_stmt|;
block|}
name|url_len
operator|=
name|strlen
argument_list|(
name|url
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
name|url_len
operator|-
literal|1
init|;
name|url
index|[
name|i
index|]
operator|==
literal|'/'
operator|&&
literal|0
operator|<=
name|i
condition|;
name|i
operator|--
control|)
empty_stmt|;
name|url_len
operator|=
name|i
operator|+
literal|1
expr_stmt|;
if|if
condition|(
literal|4
operator|<
name|i
operator|&&
operator|!
name|strncmp
argument_list|(
literal|".git"
argument_list|,
name|url
operator|+
name|i
operator|-
literal|3
argument_list|,
literal|4
argument_list|)
condition|)
name|url_len
operator|=
name|i
operator|-
literal|3
expr_stmt|;
name|note_len
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|*
name|what
condition|)
block|{
if|if
condition|(
operator|*
name|kind
condition|)
name|note_len
operator|+=
name|sprintf
argument_list|(
name|note
operator|+
name|note_len
argument_list|,
literal|"%s "
argument_list|,
name|kind
argument_list|)
expr_stmt|;
name|note_len
operator|+=
name|sprintf
argument_list|(
name|note
operator|+
name|note_len
argument_list|,
literal|"'%s' of "
argument_list|,
name|what
argument_list|)
expr_stmt|;
block|}
name|note_len
operator|+=
name|sprintf
argument_list|(
name|note
operator|+
name|note_len
argument_list|,
literal|"%.*s"
argument_list|,
name|url_len
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|fp
argument_list|,
literal|"%s\t%s\t%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
condition|?
name|commit
operator|->
name|object
operator|.
name|sha1
else|:
name|rm
operator|->
name|old_sha1
argument_list|)
argument_list|,
name|rm
operator|->
name|merge
condition|?
literal|""
else|:
literal|"not-for-merge"
argument_list|,
name|note
argument_list|)
expr_stmt|;
if|if
condition|(
name|ref
condition|)
name|update_local_ref
argument_list|(
name|ref
argument_list|,
name|note
argument_list|,
name|verbose
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fetch_refs
specifier|static
name|int
name|fetch_refs
parameter_list|(
name|struct
name|transport
modifier|*
name|transport
parameter_list|,
name|struct
name|ref
modifier|*
name|ref_map
parameter_list|)
block|{
name|int
name|ret
init|=
name|transport_fetch_refs
argument_list|(
name|transport
argument_list|,
name|ref_map
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|store_updated_refs
argument_list|(
name|transport
operator|->
name|url
argument_list|,
name|ref_map
argument_list|)
expr_stmt|;
return|return
name|ret
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
name|path_list
modifier|*
name|list
init|=
operator|(
expr|struct
name|path_list
operator|*
operator|)
name|cbdata
decl_stmt|;
name|path_list_insert
argument_list|(
name|refname
argument_list|,
name|list
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|find_non_local_tags
specifier|static
name|struct
name|ref
modifier|*
name|find_non_local_tags
parameter_list|(
name|struct
name|transport
modifier|*
name|transport
parameter_list|,
name|struct
name|ref
modifier|*
name|fetch_map
parameter_list|)
block|{
specifier|static
name|struct
name|path_list
name|existing_refs
init|=
block|{
name|NULL
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|}
decl_stmt|;
name|struct
name|path_list
name|new_refs
init|=
block|{
name|NULL
block|,
literal|0
block|,
literal|0
block|,
literal|1
block|}
decl_stmt|;
name|char
modifier|*
name|ref_name
decl_stmt|;
name|int
name|ref_name_len
decl_stmt|;
name|unsigned
name|char
modifier|*
name|ref_sha1
decl_stmt|;
name|struct
name|ref
modifier|*
name|tag_ref
decl_stmt|;
name|struct
name|ref
modifier|*
name|rm
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref_map
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
modifier|*
name|tail
init|=
operator|&
name|ref_map
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
name|for_each_ref
argument_list|(
name|add_existing
argument_list|,
operator|&
name|existing_refs
argument_list|)
expr_stmt|;
for|for
control|(
name|ref
operator|=
name|transport_get_remote_refs
argument_list|(
name|transport
argument_list|)
init|;
name|ref
condition|;
name|ref
operator|=
name|ref
operator|->
name|next
control|)
block|{
if|if
condition|(
name|prefixcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/tags"
argument_list|)
condition|)
continue|continue;
name|ref_name
operator|=
name|xstrdup
argument_list|(
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|ref_name_len
operator|=
name|strlen
argument_list|(
name|ref_name
argument_list|)
expr_stmt|;
name|ref_sha1
operator|=
name|ref
operator|->
name|old_sha1
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|ref_name
operator|+
name|ref_name_len
operator|-
literal|3
argument_list|,
literal|"^{}"
argument_list|)
condition|)
block|{
name|ref_name
index|[
name|ref_name_len
operator|-
literal|3
index|]
operator|=
literal|0
expr_stmt|;
name|tag_ref
operator|=
name|transport_get_remote_refs
argument_list|(
name|transport
argument_list|)
expr_stmt|;
while|while
condition|(
name|tag_ref
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|tag_ref
operator|->
name|name
argument_list|,
name|ref_name
argument_list|)
condition|)
block|{
name|ref_sha1
operator|=
name|tag_ref
operator|->
name|old_sha1
expr_stmt|;
break|break;
block|}
name|tag_ref
operator|=
name|tag_ref
operator|->
name|next
expr_stmt|;
block|}
block|}
if|if
condition|(
operator|!
name|path_list_has_path
argument_list|(
operator|&
name|existing_refs
argument_list|,
name|ref_name
argument_list|)
operator|&&
operator|!
name|path_list_has_path
argument_list|(
operator|&
name|new_refs
argument_list|,
name|ref_name
argument_list|)
operator|&&
name|lookup_object
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Auto-following %s\n"
argument_list|,
name|ref_name
argument_list|)
expr_stmt|;
name|path_list_insert
argument_list|(
name|ref_name
argument_list|,
operator|&
name|new_refs
argument_list|)
expr_stmt|;
name|rm
operator|=
name|alloc_ref
argument_list|(
name|strlen
argument_list|(
name|ref_name
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|rm
operator|->
name|name
argument_list|,
name|ref_name
argument_list|)
expr_stmt|;
name|rm
operator|->
name|peer_ref
operator|=
name|alloc_ref
argument_list|(
name|strlen
argument_list|(
name|ref_name
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|rm
operator|->
name|peer_ref
operator|->
name|name
argument_list|,
name|ref_name
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|rm
operator|->
name|old_sha1
argument_list|,
name|ref_sha1
argument_list|)
expr_stmt|;
operator|*
name|tail
operator|=
name|rm
expr_stmt|;
name|tail
operator|=
operator|&
name|rm
operator|->
name|next
expr_stmt|;
block|}
name|free
argument_list|(
name|ref_name
argument_list|)
expr_stmt|;
block|}
return|return
name|ref_map
return|;
block|}
end_function
begin_function
DECL|function|do_fetch
specifier|static
name|int
name|do_fetch
parameter_list|(
name|struct
name|transport
modifier|*
name|transport
parameter_list|,
name|struct
name|refspec
modifier|*
name|refs
parameter_list|,
name|int
name|ref_count
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|ref_map
decl_stmt|,
modifier|*
name|fetch_map
decl_stmt|;
name|struct
name|ref
modifier|*
name|rm
decl_stmt|;
name|int
name|autotags
init|=
operator|(
name|transport
operator|->
name|remote
operator|->
name|fetch_tags
operator|==
literal|1
operator|)
decl_stmt|;
if|if
condition|(
name|transport
operator|->
name|remote
operator|->
name|fetch_tags
operator|==
literal|2
operator|&&
operator|!
name|no_tags
condition|)
name|tags
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|transport
operator|->
name|remote
operator|->
name|fetch_tags
operator|==
operator|-
literal|1
condition|)
name|no_tags
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|transport
operator|->
name|ops
operator|||
operator|!
name|transport
operator|->
name|ops
operator|->
name|get_refs_list
operator|||
operator|!
name|transport
operator|->
name|ops
operator|->
name|fetch
condition|)
name|die
argument_list|(
literal|"Don't know how to fetch from %s"
argument_list|,
name|transport
operator|->
name|url
argument_list|)
expr_stmt|;
comment|/* if not appending, truncate FETCH_HEAD */
if|if
condition|(
operator|!
name|append
condition|)
name|fclose
argument_list|(
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"FETCH_HEAD"
argument_list|)
argument_list|,
literal|"w"
argument_list|)
argument_list|)
expr_stmt|;
name|ref_map
operator|=
name|get_ref_map
argument_list|(
name|transport
argument_list|,
name|refs
argument_list|,
name|ref_count
argument_list|,
name|tags
argument_list|,
operator|&
name|autotags
argument_list|)
expr_stmt|;
for|for
control|(
name|rm
operator|=
name|ref_map
init|;
name|rm
condition|;
name|rm
operator|=
name|rm
operator|->
name|next
control|)
block|{
if|if
condition|(
name|rm
operator|->
name|peer_ref
condition|)
name|read_ref
argument_list|(
name|rm
operator|->
name|peer_ref
operator|->
name|name
argument_list|,
name|rm
operator|->
name|peer_ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|fetch_refs
argument_list|(
name|transport
argument_list|,
name|ref_map
argument_list|)
condition|)
block|{
name|free_refs
argument_list|(
name|ref_map
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|fetch_map
operator|=
name|ref_map
expr_stmt|;
comment|/* if neither --no-tags nor --tags was specified, do automated tag 	 * following ... */
if|if
condition|(
operator|!
operator|(
name|tags
operator|||
name|no_tags
operator|)
operator|&&
name|autotags
condition|)
block|{
name|ref_map
operator|=
name|find_non_local_tags
argument_list|(
name|transport
argument_list|,
name|fetch_map
argument_list|)
expr_stmt|;
if|if
condition|(
name|ref_map
condition|)
block|{
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_DEPTH
argument_list|,
literal|"0"
argument_list|)
expr_stmt|;
name|fetch_refs
argument_list|(
name|transport
argument_list|,
name|ref_map
argument_list|)
expr_stmt|;
block|}
name|free_refs
argument_list|(
name|ref_map
argument_list|)
expr_stmt|;
block|}
name|free_refs
argument_list|(
name|fetch_map
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_fetch
name|int
name|cmd_fetch
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
name|remote
modifier|*
name|remote
decl_stmt|;
name|struct
name|transport
modifier|*
name|transport
decl_stmt|;
name|int
name|i
decl_stmt|,
name|j
decl_stmt|,
name|rla_offset
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|refs
init|=
name|NULL
decl_stmt|;
name|int
name|ref_nr
init|=
literal|0
decl_stmt|;
name|int
name|cmd_len
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|depth
init|=
name|NULL
decl_stmt|,
modifier|*
name|upload_pack
init|=
name|NULL
decl_stmt|;
name|int
name|keep
init|=
literal|0
decl_stmt|;
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
name|cmd_len
operator|+=
name|strlen
argument_list|(
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
break|break;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--append"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-a"
argument_list|)
condition|)
block|{
name|append
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--upload-pack="
argument_list|)
condition|)
block|{
name|upload_pack
operator|=
name|arg
operator|+
literal|14
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
literal|"--upload-pack"
argument_list|)
condition|)
block|{
name|i
operator|++
expr_stmt|;
if|if
condition|(
name|i
operator|==
name|argc
condition|)
name|usage
argument_list|(
name|fetch_usage
argument_list|)
expr_stmt|;
name|upload_pack
operator|=
name|argv
index|[
name|i
index|]
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
literal|"--force"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-f"
argument_list|)
condition|)
block|{
name|force
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
literal|"--no-tags"
argument_list|)
condition|)
block|{
name|no_tags
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
literal|"--tags"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-t"
argument_list|)
condition|)
block|{
name|tags
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
literal|"--keep"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-k"
argument_list|)
condition|)
block|{
name|keep
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
literal|"--update-head-ok"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-u"
argument_list|)
condition|)
block|{
name|update_head_ok
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--depth="
argument_list|)
condition|)
block|{
name|depth
operator|=
name|arg
operator|+
literal|8
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
literal|"--depth"
argument_list|)
condition|)
block|{
name|i
operator|++
expr_stmt|;
if|if
condition|(
name|i
operator|==
name|argc
condition|)
name|usage
argument_list|(
name|fetch_usage
argument_list|)
expr_stmt|;
name|depth
operator|=
name|argv
index|[
name|i
index|]
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
literal|"--quiet"
argument_list|)
condition|)
block|{
name|quiet
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
literal|"--verbose"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-v"
argument_list|)
condition|)
block|{
name|verbose
operator|++
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|fetch_usage
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|j
operator|=
name|i
init|;
name|j
operator|<
name|argc
condition|;
name|j
operator|++
control|)
name|cmd_len
operator|+=
name|strlen
argument_list|(
name|argv
index|[
name|j
index|]
argument_list|)
expr_stmt|;
name|default_rla
operator|=
name|xmalloc
argument_list|(
name|cmd_len
operator|+
literal|5
operator|+
name|argc
operator|+
literal|1
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|default_rla
argument_list|,
literal|"fetch"
argument_list|)
expr_stmt|;
name|rla_offset
operator|=
name|strlen
argument_list|(
name|default_rla
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|1
init|;
name|j
operator|<
name|argc
condition|;
name|j
operator|++
control|)
block|{
name|sprintf
argument_list|(
name|default_rla
operator|+
name|rla_offset
argument_list|,
literal|" %s"
argument_list|,
name|argv
index|[
name|j
index|]
argument_list|)
expr_stmt|;
name|rla_offset
operator|+=
name|strlen
argument_list|(
name|argv
index|[
name|j
index|]
argument_list|)
operator|+
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|i
operator|==
name|argc
condition|)
name|remote
operator|=
name|remote_get
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
else|else
name|remote
operator|=
name|remote_get
argument_list|(
name|argv
index|[
name|i
operator|++
index|]
argument_list|)
expr_stmt|;
name|transport
operator|=
name|transport_get
argument_list|(
name|remote
argument_list|,
name|remote
operator|->
name|uri
index|[
literal|0
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose
operator|>=
literal|2
condition|)
name|transport
operator|->
name|verbose
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|quiet
condition|)
name|transport
operator|->
name|verbose
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|upload_pack
condition|)
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_UPLOADPACK
argument_list|,
name|upload_pack
argument_list|)
expr_stmt|;
if|if
condition|(
name|keep
condition|)
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_KEEP
argument_list|,
literal|"yes"
argument_list|)
expr_stmt|;
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_DEPTH
argument_list|,
name|depth
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|transport
operator|->
name|url
condition|)
name|die
argument_list|(
literal|"Where do you want to fetch from today?"
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|<
name|argc
condition|)
block|{
name|int
name|j
init|=
literal|0
decl_stmt|;
name|refs
operator|=
name|xcalloc
argument_list|(
name|argc
operator|-
name|i
operator|+
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|argc
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"tag"
argument_list|)
condition|)
block|{
name|char
modifier|*
name|ref
decl_stmt|;
name|i
operator|++
expr_stmt|;
name|ref
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|)
operator|*
literal|2
operator|+
literal|22
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ref
argument_list|,
literal|"refs/tags/"
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|ref
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|ref
argument_list|,
literal|":refs/tags/"
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|ref
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|refs
index|[
name|j
operator|++
index|]
operator|=
name|ref
expr_stmt|;
block|}
else|else
name|refs
index|[
name|j
operator|++
index|]
operator|=
name|argv
index|[
name|i
index|]
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
name|refs
index|[
name|j
index|]
operator|=
name|NULL
expr_stmt|;
name|ref_nr
operator|=
name|j
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|refs
index|[
name|j
index|]
condition|;
name|j
operator|++
control|)
name|printf
argument_list|(
literal|"ref: %s\n"
argument_list|,
name|refs
index|[
name|j
index|]
argument_list|)
expr_stmt|;
block|}
return|return
name|do_fetch
argument_list|(
name|transport
argument_list|,
name|parse_ref_spec
argument_list|(
name|ref_nr
argument_list|,
name|refs
argument_list|)
argument_list|,
name|ref_nr
argument_list|)
return|;
block|}
end_function
end_unit
