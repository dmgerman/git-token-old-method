begin_unit
begin_include
include|#
directive|include
file|"pull.h"
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
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_decl_stmt
DECL|variable|write_ref
specifier|const
name|char
modifier|*
name|write_ref
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|current_ref
specifier|const
name|unsigned
name|char
modifier|*
name|current_ref
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_tree
name|int
name|get_tree
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_history
name|int
name|get_history
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_all
name|int
name|get_all
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_verbosely
name|int
name|get_verbosely
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|current_commit_sha1
specifier|static
name|unsigned
name|char
name|current_commit_sha1
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|pull_say
name|void
name|pull_say
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
specifier|const
name|char
modifier|*
name|hex
parameter_list|)
block|{
if|if
condition|(
name|get_verbosely
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|fmt
argument_list|,
name|hex
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|report_missing
specifier|static
name|void
name|report_missing
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|missing
parameter_list|)
block|{
name|char
name|missing_hex
index|[
literal|41
index|]
decl_stmt|;
name|strcpy
argument_list|(
name|missing_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|missing
argument_list|)
argument_list|)
expr_stmt|;
empty_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Cannot obtain needed %s %s\nwhile processing commit %s.\n"
argument_list|,
name|what
argument_list|,
name|missing_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|current_commit_sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|make_sure_we_have_it
specifier|static
name|int
name|make_sure_we_have_it
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|status
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
block|{
name|status
operator|=
name|fetch
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
operator|&&
name|what
condition|)
name|report_missing
argument_list|(
name|what
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
return|return
name|status
return|;
block|}
end_function
begin_function_decl
specifier|static
name|int
name|process
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|process_tree
specifier|static
name|int
name|process_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|)
block|{
name|struct
name|tree_entry_list
modifier|*
name|entries
decl_stmt|;
if|if
condition|(
name|parse_tree
argument_list|(
name|tree
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|entries
operator|=
name|tree
operator|->
name|entries
init|;
name|entries
condition|;
name|entries
operator|=
name|entries
operator|->
name|next
control|)
block|{
if|if
condition|(
name|process
argument_list|(
name|entries
operator|->
name|item
operator|.
name|any
operator|->
name|sha1
argument_list|,
name|entries
operator|->
name|directory
condition|?
name|tree_type
else|:
name|blob_type
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|process_commit
specifier|static
name|int
name|process_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
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
name|memcpy
argument_list|(
name|current_commit_sha1
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
if|if
condition|(
name|get_tree
condition|)
block|{
if|if
condition|(
name|process
argument_list|(
name|commit
operator|->
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|tree_type
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|get_all
condition|)
name|get_tree
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|get_history
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
for|for
control|(
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
control|)
block|{
if|if
condition|(
name|has_sha1_file
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|process
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|commit_type
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|process_tag
specifier|static
name|int
name|process_tag
parameter_list|(
name|struct
name|tag
modifier|*
name|tag
parameter_list|)
block|{
if|if
condition|(
name|parse_tag
argument_list|(
name|tag
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|process
argument_list|(
name|tag
operator|->
name|tagged
operator|->
name|sha1
argument_list|,
name|NULL
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|process_queue
specifier|static
name|struct
name|object_list
modifier|*
name|process_queue
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|process_queue_end
specifier|static
name|struct
name|object_list
modifier|*
modifier|*
name|process_queue_end
init|=
operator|&
name|process_queue
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|process_object
specifier|static
name|int
name|process_object
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|commit_type
condition|)
block|{
if|if
condition|(
name|process_commit
argument_list|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|tree_type
condition|)
block|{
if|if
condition|(
name|process_tree
argument_list|(
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|blob_type
condition|)
block|{
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|tag_type
condition|)
block|{
if|if
condition|(
name|process_tag
argument_list|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
return|return
name|error
argument_list|(
literal|"Unable to determine requirements "
literal|"of type %s for %s"
argument_list|,
name|obj
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|process
specifier|static
name|int
name|process
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object_type
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|)
decl_stmt|;
if|if
condition|(
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
block|{
comment|/* We already have it, so we should scan it now. */
return|return
name|process_object
argument_list|(
name|obj
argument_list|)
return|;
block|}
if|if
condition|(
name|object_list_contains
argument_list|(
name|process_queue
argument_list|,
name|obj
argument_list|)
condition|)
return|return
literal|0
return|;
name|object_list_insert
argument_list|(
name|obj
argument_list|,
name|process_queue_end
argument_list|)
expr_stmt|;
name|process_queue_end
operator|=
operator|&
operator|(
operator|*
name|process_queue_end
operator|)
operator|->
name|next
expr_stmt|;
comment|//fprintf(stderr, "prefetch %s\n", sha1_to_hex(sha1));
name|prefetch
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|loop
specifier|static
name|int
name|loop
parameter_list|(
name|void
parameter_list|)
block|{
while|while
condition|(
name|process_queue
condition|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|process_queue
operator|->
name|item
decl_stmt|;
comment|/* 		fprintf(stderr, "%d objects to pull\n",  			object_list_length(process_queue)); 		*/
name|process_queue
operator|=
name|process_queue
operator|->
name|next
expr_stmt|;
if|if
condition|(
operator|!
name|process_queue
condition|)
name|process_queue_end
operator|=
operator|&
name|process_queue
expr_stmt|;
comment|//fprintf(stderr, "fetch %s\n", sha1_to_hex(obj->sha1));
if|if
condition|(
name|make_sure_we_have_it
argument_list|(
name|obj
operator|->
name|type
condition|?
else|:
literal|"object"
argument_list|,
name|obj
operator|->
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|obj
operator|->
name|type
condition|)
name|parse_object
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|process_object
argument_list|(
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|interpret_target
specifier|static
name|int
name|interpret_target
parameter_list|(
name|char
modifier|*
name|target
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|target
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|check_ref_format
argument_list|(
name|target
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|fetch_ref
argument_list|(
name|target
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
return|return
literal|0
return|;
block|}
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|pull
name|int
name|pull
parameter_list|(
name|char
modifier|*
name|target
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
name|fd
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|write_ref
operator|&&
name|current_ref
condition|)
block|{
name|fd
operator|=
name|lock_ref_sha1
argument_list|(
name|write_ref
argument_list|,
name|current_ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|interpret_target
argument_list|(
name|target
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not interpret %s as something to pull"
argument_list|,
name|target
argument_list|)
return|;
if|if
condition|(
name|process
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|loop
argument_list|()
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|write_ref
condition|)
block|{
if|if
condition|(
name|current_ref
condition|)
block|{
name|write_ref_sha1
argument_list|(
name|write_ref
argument_list|,
name|fd
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|write_ref_sha1_unlocked
argument_list|(
name|write_ref
argument_list|,
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
end_unit
