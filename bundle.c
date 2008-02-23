begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"bundle.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
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
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_decl_stmt
DECL|variable|bundle_signature
specifier|static
specifier|const
name|char
name|bundle_signature
index|[]
init|=
literal|"# v2 git bundle\n"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_to_ref_list
specifier|static
name|void
name|add_to_ref_list
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|ref_list
modifier|*
name|list
parameter_list|)
block|{
if|if
condition|(
name|list
operator|->
name|nr
operator|+
literal|1
operator|>=
name|list
operator|->
name|alloc
condition|)
block|{
name|list
operator|->
name|alloc
operator|=
name|alloc_nr
argument_list|(
name|list
operator|->
name|nr
operator|+
literal|1
argument_list|)
expr_stmt|;
name|list
operator|->
name|list
operator|=
name|xrealloc
argument_list|(
name|list
operator|->
name|list
argument_list|,
name|list
operator|->
name|alloc
operator|*
sizeof|sizeof
argument_list|(
name|list
operator|->
name|list
index|[
literal|0
index|]
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|list
operator|->
name|list
index|[
name|list
operator|->
name|nr
index|]
operator|.
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|list
operator|->
name|list
index|[
name|list
operator|->
name|nr
index|]
operator|.
name|name
operator|=
name|xstrdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|list
operator|->
name|nr
operator|++
expr_stmt|;
block|}
end_function
begin_comment
comment|/* returns an fd */
end_comment
begin_function
DECL|function|read_bundle_header
name|int
name|read_bundle_header
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|)
block|{
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|long
name|fpos
decl_stmt|;
name|FILE
modifier|*
name|ffd
init|=
name|fopen
argument_list|(
name|path
argument_list|,
literal|"rb"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ffd
condition|)
return|return
name|error
argument_list|(
literal|"could not open '%s'"
argument_list|,
name|path
argument_list|)
return|;
if|if
condition|(
operator|!
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|ffd
argument_list|)
operator|||
name|strcmp
argument_list|(
name|buffer
argument_list|,
name|bundle_signature
argument_list|)
condition|)
block|{
name|fclose
argument_list|(
name|ffd
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"'%s' does not look like a v2 bundle file"
argument_list|,
name|path
argument_list|)
return|;
block|}
while|while
condition|(
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|ffd
argument_list|)
operator|&&
name|buffer
index|[
literal|0
index|]
operator|!=
literal|'\n'
condition|)
block|{
name|int
name|is_prereq
init|=
name|buffer
index|[
literal|0
index|]
operator|==
literal|'-'
decl_stmt|;
name|int
name|offset
init|=
name|is_prereq
condition|?
literal|1
else|:
literal|0
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|buffer
argument_list|)
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|ref_list
modifier|*
name|list
init|=
name|is_prereq
condition|?
operator|&
name|header
operator|->
name|prerequisites
else|:
operator|&
name|header
operator|->
name|references
decl_stmt|;
name|char
name|delim
decl_stmt|;
if|if
condition|(
name|len
operator|&&
name|buffer
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buffer
index|[
name|len
operator|-
literal|1
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|warning
argument_list|(
literal|"unrecognized header: %s"
argument_list|,
name|buffer
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|delim
operator|=
name|buffer
index|[
literal|40
operator|+
name|offset
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|isspace
argument_list|(
name|delim
argument_list|)
operator|&&
operator|(
name|delim
operator|!=
literal|'\0'
operator|||
operator|!
name|is_prereq
operator|)
condition|)
name|die
argument_list|(
literal|"invalid header: %s"
argument_list|,
name|buffer
argument_list|)
expr_stmt|;
name|add_to_ref_list
argument_list|(
name|sha1
argument_list|,
name|isspace
argument_list|(
name|delim
argument_list|)
condition|?
name|buffer
operator|+
literal|41
operator|+
name|offset
else|:
literal|""
argument_list|,
name|list
argument_list|)
expr_stmt|;
block|}
name|fpos
operator|=
name|ftell
argument_list|(
name|ffd
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|ffd
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"could not open '%s'"
argument_list|,
name|path
argument_list|)
return|;
name|lseek
argument_list|(
name|fd
argument_list|,
name|fpos
argument_list|,
name|SEEK_SET
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|list_refs
specifier|static
name|int
name|list_refs
parameter_list|(
name|struct
name|ref_list
modifier|*
name|r
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
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
name|r
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
block|{
name|int
name|j
decl_stmt|;
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|r
operator|->
name|list
index|[
name|i
index|]
operator|.
name|name
argument_list|,
name|argv
index|[
name|j
index|]
argument_list|)
condition|)
break|break;
if|if
condition|(
name|j
operator|==
name|argc
condition|)
continue|continue;
block|}
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|r
operator|->
name|list
index|[
name|i
index|]
operator|.
name|sha1
argument_list|)
argument_list|,
name|r
operator|->
name|list
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_define
DECL|macro|PREREQ_MARK
define|#
directive|define
name|PREREQ_MARK
value|(1u<<16)
end_define
begin_function
DECL|function|verify_bundle
name|int
name|verify_bundle
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
name|int
name|verbose
parameter_list|)
block|{
comment|/* 	 * Do fast check, then if any prereqs are missing then go line by line 	 * to be verbose about the errors 	 */
name|struct
name|ref_list
modifier|*
name|p
init|=
operator|&
name|header
operator|->
name|prerequisites
decl_stmt|;
name|struct
name|rev_info
name|revs
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
name|NULL
block|,
literal|"--all"
block|}
decl_stmt|;
name|struct
name|object_array
name|refs
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|int
name|i
decl_stmt|,
name|ret
init|=
literal|0
decl_stmt|,
name|req_nr
decl_stmt|;
specifier|const
name|char
modifier|*
name|message
init|=
literal|"Repository lacks these prerequisite commits:"
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|NULL
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
name|p
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|ref_list_entry
modifier|*
name|e
init|=
name|p
operator|->
name|list
operator|+
name|i
decl_stmt|;
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|o
condition|)
block|{
name|o
operator|->
name|flags
operator||=
name|PREREQ_MARK
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|revs
argument_list|,
name|o
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|++
name|ret
operator|==
literal|1
condition|)
name|error
argument_list|(
name|message
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"%s %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|revs
operator|.
name|pending
operator|.
name|nr
operator|!=
name|p
operator|->
name|nr
condition|)
return|return
name|ret
return|;
name|req_nr
operator|=
name|revs
operator|.
name|pending
operator|.
name|nr
expr_stmt|;
name|setup_revisions
argument_list|(
literal|2
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
name|refs
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|object_array
argument_list|)
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
name|revs
operator|.
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_array_entry
modifier|*
name|e
init|=
name|revs
operator|.
name|pending
operator|.
name|objects
operator|+
name|i
decl_stmt|;
name|add_object_array
argument_list|(
name|e
operator|->
name|item
argument_list|,
name|e
operator|->
name|name
argument_list|,
operator|&
name|refs
argument_list|)
expr_stmt|;
block|}
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
name|i
operator|=
name|req_nr
expr_stmt|;
while|while
condition|(
name|i
operator|&&
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
operator|&
name|revs
argument_list|)
operator|)
condition|)
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|PREREQ_MARK
condition|)
name|i
operator|--
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|req_nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
operator|(
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
operator|->
name|flags
operator|&
name|SHOWN
operator|)
condition|)
block|{
if|if
condition|(
operator|++
name|ret
operator|==
literal|1
condition|)
name|error
argument_list|(
name|message
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"%s %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
operator|->
name|sha1
argument_list|)
argument_list|,
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|refs
operator|.
name|nr
condition|;
name|i
operator|++
control|)
name|clear_commit_marks
argument_list|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
argument_list|,
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose
condition|)
block|{
name|struct
name|ref_list
modifier|*
name|r
decl_stmt|;
name|r
operator|=
operator|&
name|header
operator|->
name|references
expr_stmt|;
name|printf
argument_list|(
literal|"The bundle contains %d ref%s\n"
argument_list|,
name|r
operator|->
name|nr
argument_list|,
operator|(
literal|1
operator|<
name|r
operator|->
name|nr
operator|)
condition|?
literal|"s"
else|:
literal|""
argument_list|)
expr_stmt|;
name|list_refs
argument_list|(
name|r
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|r
operator|=
operator|&
name|header
operator|->
name|prerequisites
expr_stmt|;
name|printf
argument_list|(
literal|"The bundle requires these %d ref%s\n"
argument_list|,
name|r
operator|->
name|nr
argument_list|,
operator|(
literal|1
operator|<
name|r
operator|->
name|nr
operator|)
condition|?
literal|"s"
else|:
literal|""
argument_list|)
expr_stmt|;
name|list_refs
argument_list|(
name|r
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|list_bundle_refs
name|int
name|list_bundle_refs
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
return|return
name|list_refs
argument_list|(
operator|&
name|header
operator|->
name|references
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|create_bundle
name|int
name|create_bundle
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|static
name|struct
name|lock_file
name|lock
decl_stmt|;
name|int
name|bundle_fd
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|bundle_to_stdout
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|argv_boundary
init|=
name|xmalloc
argument_list|(
operator|(
name|argc
operator|+
literal|4
operator|)
operator|*
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|argv_pack
init|=
name|xmalloc
argument_list|(
literal|5
operator|*
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|i
decl_stmt|,
name|ref_count
init|=
literal|0
decl_stmt|;
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|struct
name|rev_info
name|revs
decl_stmt|;
name|struct
name|child_process
name|rls
decl_stmt|;
name|FILE
modifier|*
name|rls_fout
decl_stmt|;
name|bundle_to_stdout
operator|=
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"-"
argument_list|)
expr_stmt|;
if|if
condition|(
name|bundle_to_stdout
condition|)
name|bundle_fd
operator|=
literal|1
expr_stmt|;
else|else
name|bundle_fd
operator|=
name|hold_lock_file_for_update
argument_list|(
operator|&
name|lock
argument_list|,
name|path
argument_list|,
literal|1
argument_list|)
expr_stmt|;
comment|/* write signature */
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|bundle_signature
argument_list|,
name|strlen
argument_list|(
name|bundle_signature
argument_list|)
argument_list|)
expr_stmt|;
comment|/* init revs to list objects for pack-objects later */
name|save_commit_buffer
operator|=
literal|0
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
comment|/* write prerequisites */
name|memcpy
argument_list|(
name|argv_boundary
operator|+
literal|3
argument_list|,
name|argv
operator|+
literal|1
argument_list|,
name|argc
operator|*
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|argv_boundary
index|[
literal|0
index|]
operator|=
literal|"rev-list"
expr_stmt|;
name|argv_boundary
index|[
literal|1
index|]
operator|=
literal|"--boundary"
expr_stmt|;
name|argv_boundary
index|[
literal|2
index|]
operator|=
literal|"--pretty=oneline"
expr_stmt|;
name|argv_boundary
index|[
name|argc
operator|+
literal|2
index|]
operator|=
name|NULL
expr_stmt|;
name|memset
argument_list|(
operator|&
name|rls
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|rls
argument_list|)
argument_list|)
expr_stmt|;
name|rls
operator|.
name|argv
operator|=
name|argv_boundary
expr_stmt|;
name|rls
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|rls
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|rls
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|rls_fout
operator|=
name|fdopen
argument_list|(
name|rls
operator|.
name|out
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|rls_fout
argument_list|)
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
name|buffer
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|buffer
argument_list|,
name|strlen
argument_list|(
name|buffer
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|buffer
operator|+
literal|1
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|object
operator|->
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|revs
argument_list|,
name|object
argument_list|,
name|buffer
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|buffer
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|object
operator|->
name|flags
operator||=
name|SHOWN
expr_stmt|;
block|}
block|}
name|fclose
argument_list|(
name|rls_fout
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|rls
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"rev-list died"
argument_list|)
return|;
comment|/* write references */
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
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
return|return
name|error
argument_list|(
literal|"unrecognized argument: %s'"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
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
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_array_entry
modifier|*
name|e
init|=
name|revs
operator|.
name|pending
operator|.
name|objects
operator|+
name|i
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|ref
decl_stmt|;
specifier|const
name|char
modifier|*
name|display_ref
decl_stmt|;
name|int
name|flag
decl_stmt|;
if|if
condition|(
name|e
operator|->
name|item
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
if|if
condition|(
name|dwim_ref
argument_list|(
name|e
operator|->
name|name
argument_list|,
name|strlen
argument_list|(
name|e
operator|->
name|name
argument_list|)
argument_list|,
name|sha1
argument_list|,
operator|&
name|ref
argument_list|)
operator|!=
literal|1
condition|)
continue|continue;
if|if
condition|(
operator|!
name|resolve_ref
argument_list|(
name|e
operator|->
name|name
argument_list|,
name|sha1
argument_list|,
literal|1
argument_list|,
operator|&
name|flag
argument_list|)
condition|)
name|flag
operator|=
literal|0
expr_stmt|;
name|display_ref
operator|=
operator|(
name|flag
operator|&
name|REF_ISSYMREF
operator|)
condition|?
name|e
operator|->
name|name
else|:
name|ref
expr_stmt|;
comment|/* 		 * Make sure the refs we wrote out is correct; --max-count and 		 * other limiting options could have prevented all the tips 		 * from getting output. 		 * 		 * Non commit objects such as tags and blobs do not have 		 * this issue as they are not affected by those extra 		 * constraints. 		 */
if|if
condition|(
operator|!
operator|(
name|e
operator|->
name|item
operator|->
name|flags
operator|&
name|SHOWN
operator|)
operator|&&
name|e
operator|->
name|item
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
block|{
name|warning
argument_list|(
literal|"ref '%s' is excluded by the rev-list options"
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* 		 * If you run "git bundle create bndl v1.0..v2.0", the 		 * name of the positive ref is "v2.0" but that is the 		 * commit that is referenced by the tag, and not the tag 		 * itself. 		 */
if|if
condition|(
name|hashcmp
argument_list|(
name|sha1
argument_list|,
name|e
operator|->
name|item
operator|->
name|sha1
argument_list|)
condition|)
block|{
comment|/* 			 * Is this the positive end of a range expressed 			 * in terms of a tag (e.g. v2.0 from the range 			 * "v1.0..v2.0")? 			 */
name|struct
name|commit
modifier|*
name|one
init|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
if|if
condition|(
name|e
operator|->
name|item
operator|==
operator|&
operator|(
name|one
operator|->
name|object
operator|)
condition|)
block|{
comment|/* 				 * Need to include e->name as an 				 * independent ref to the pack-objects 				 * input, so that the tag is included 				 * in the output; otherwise we would 				 * end up triggering "empty bundle" 				 * error. 				 */
name|obj
operator|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|obj
operator|->
name|flags
operator||=
name|SHOWN
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|revs
argument_list|,
name|obj
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|ref_count
operator|++
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|sha1_to_hex
argument_list|(
name|e
operator|->
name|item
operator|->
name|sha1
argument_list|)
argument_list|,
literal|40
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
literal|" "
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|display_ref
argument_list|,
name|strlen
argument_list|(
name|display_ref
argument_list|)
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|ref_count
condition|)
name|die
argument_list|(
literal|"Refusing to create empty bundle."
argument_list|)
expr_stmt|;
comment|/* end header */
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
comment|/* write pack */
name|argv_pack
index|[
literal|0
index|]
operator|=
literal|"pack-objects"
expr_stmt|;
name|argv_pack
index|[
literal|1
index|]
operator|=
literal|"--all-progress"
expr_stmt|;
name|argv_pack
index|[
literal|2
index|]
operator|=
literal|"--stdout"
expr_stmt|;
name|argv_pack
index|[
literal|3
index|]
operator|=
literal|"--thin"
expr_stmt|;
name|argv_pack
index|[
literal|4
index|]
operator|=
name|NULL
expr_stmt|;
name|memset
argument_list|(
operator|&
name|rls
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|rls
argument_list|)
argument_list|)
expr_stmt|;
name|rls
operator|.
name|argv
operator|=
name|argv_pack
expr_stmt|;
name|rls
operator|.
name|in
operator|=
operator|-
literal|1
expr_stmt|;
name|rls
operator|.
name|out
operator|=
name|bundle_fd
expr_stmt|;
name|rls
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|rls
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not spawn pack-objects"
argument_list|)
return|;
comment|/* 	 * start_command closed bundle_fd if it was> 1 	 * so set the lock fd to -1 so commit_lock_file() 	 * won't fail trying to close it. 	 */
name|lock
operator|.
name|fd
operator|=
operator|-
literal|1
expr_stmt|;
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
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|revs
operator|.
name|pending
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
decl_stmt|;
if|if
condition|(
name|object
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
name|write_or_die
argument_list|(
name|rls
operator|.
name|in
argument_list|,
literal|"^"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|rls
operator|.
name|in
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
operator|->
name|sha1
argument_list|)
argument_list|,
literal|40
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|rls
operator|.
name|in
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|rls
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"pack-objects died"
argument_list|)
return|;
return|return
name|bundle_to_stdout
condition|?
name|close
argument_list|(
name|bundle_fd
argument_list|)
else|:
name|commit_lock_file
argument_list|(
operator|&
name|lock
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|unbundle
name|int
name|unbundle
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
name|int
name|bundle_fd
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|argv_index_pack
index|[]
init|=
block|{
literal|"index-pack"
block|,
literal|"--fix-thin"
block|,
literal|"--stdin"
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|child_process
name|ip
decl_stmt|;
if|if
condition|(
name|verify_bundle
argument_list|(
name|header
argument_list|,
literal|0
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|memset
argument_list|(
operator|&
name|ip
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|ip
argument_list|)
argument_list|)
expr_stmt|;
name|ip
operator|.
name|argv
operator|=
name|argv_index_pack
expr_stmt|;
name|ip
operator|.
name|in
operator|=
name|bundle_fd
expr_stmt|;
name|ip
operator|.
name|no_stdout
operator|=
literal|1
expr_stmt|;
name|ip
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|run_command
argument_list|(
operator|&
name|ip
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"index-pack died"
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
