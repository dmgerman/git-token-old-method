begin_unit
begin_comment
comment|/*  * test-revision-walking.c: test revision walking API.  *  * (C) 2012 Heiko Voigt<hvoigt@hvoigt.net>  *  * This code is free software; you can redistribute it and/or modify  * it under the terms of the GNU General Public License version 2 as  * published by the Free Software Foundation.  */
end_comment
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
begin_function
DECL|function|print_commit
specifier|static
name|void
name|print_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
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
name|date_mode
operator|.
name|type
operator|=
name|DATE_NORMAL
expr_stmt|;
name|format_commit_message
argument_list|(
name|commit
argument_list|,
literal|" %m %s"
argument_list|,
operator|&
name|sb
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|run_revision_walk
specifier|static
name|int
name|run_revision_walk
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
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
block|,
name|NULL
block|}
decl_stmt|;
name|int
name|argc
init|=
name|ARRAY_SIZE
argument_list|(
name|argv
argument_list|)
operator|-
literal|1
decl_stmt|;
name|int
name|got_revision
init|=
literal|0
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|prepare_revision_walk
argument_list|(
operator|&
name|rev
argument_list|)
condition|)
name|die
argument_list|(
literal|"revision walk setup failed"
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
operator|&
name|rev
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|print_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
name|got_revision
operator|=
literal|1
expr_stmt|;
block|}
name|reset_revision_walk
argument_list|()
expr_stmt|;
return|return
name|got_revision
return|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
return|return
literal|1
return|;
name|setup_git_directory
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"run-twice"
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"1st\n"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|run_revision_walk
argument_list|()
condition|)
return|return
literal|1
return|;
name|printf
argument_list|(
literal|"2nd\n"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|run_revision_walk
argument_list|()
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"check usage\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
end_unit