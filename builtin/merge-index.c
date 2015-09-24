begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_decl_stmt
DECL|variable|pgm
specifier|static
specifier|const
name|char
modifier|*
name|pgm
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|one_shot
DECL|variable|quiet
specifier|static
name|int
name|one_shot
decl_stmt|,
name|quiet
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|err
specifier|static
name|int
name|err
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|merge_entry
specifier|static
name|int
name|merge_entry
parameter_list|(
name|int
name|pos
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|found
decl_stmt|;
specifier|const
name|char
modifier|*
name|arguments
index|[]
init|=
block|{
name|pgm
block|,
literal|""
block|,
literal|""
block|,
literal|""
block|,
name|path
block|,
literal|""
block|,
literal|""
block|,
literal|""
block|,
name|NULL
block|}
decl_stmt|;
name|char
name|hexbuf
index|[
literal|4
index|]
index|[
literal|60
index|]
decl_stmt|;
name|char
name|ownbuf
index|[
literal|4
index|]
index|[
literal|60
index|]
decl_stmt|;
if|if
condition|(
name|pos
operator|>=
name|active_nr
condition|)
name|die
argument_list|(
literal|"git merge-index: %s not in the cache"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|found
operator|=
literal|0
expr_stmt|;
do|do
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|pos
index|]
decl_stmt|;
name|int
name|stage
init|=
name|ce_stage
argument_list|(
name|ce
argument_list|)
decl_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|path
argument_list|)
condition|)
break|break;
name|found
operator|++
expr_stmt|;
name|strcpy
argument_list|(
name|hexbuf
index|[
name|stage
index|]
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|xsnprintf
argument_list|(
name|ownbuf
index|[
name|stage
index|]
argument_list|,
sizeof|sizeof
argument_list|(
name|ownbuf
index|[
name|stage
index|]
argument_list|)
argument_list|,
literal|"%o"
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|arguments
index|[
name|stage
index|]
operator|=
name|hexbuf
index|[
name|stage
index|]
expr_stmt|;
name|arguments
index|[
name|stage
operator|+
literal|4
index|]
operator|=
name|ownbuf
index|[
name|stage
index|]
expr_stmt|;
block|}
do|while
condition|(
operator|++
name|pos
operator|<
name|active_nr
condition|)
do|;
if|if
condition|(
operator|!
name|found
condition|)
name|die
argument_list|(
literal|"git merge-index: %s not in the cache"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|arguments
argument_list|,
literal|0
argument_list|)
condition|)
block|{
if|if
condition|(
name|one_shot
condition|)
name|err
operator|++
expr_stmt|;
else|else
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|die
argument_list|(
literal|"merge program failed"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|found
return|;
block|}
end_function
begin_function
DECL|function|merge_one_path
specifier|static
name|void
name|merge_one_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|path
argument_list|,
name|strlen
argument_list|(
name|path
argument_list|)
argument_list|)
decl_stmt|;
comment|/* 	 * If it already exists in the cache as stage0, it's 	 * already merged and there is nothing to do. 	 */
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
name|merge_entry
argument_list|(
operator|-
name|pos
operator|-
literal|1
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|merge_all
specifier|static
name|void
name|merge_all
parameter_list|(
name|void
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
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
name|i
operator|+=
name|merge_entry
argument_list|(
name|i
argument_list|,
name|ce
operator|->
name|name
argument_list|)
operator|-
literal|1
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmd_merge_index
name|int
name|cmd_merge_index
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
decl_stmt|,
name|force_file
init|=
literal|0
decl_stmt|;
comment|/* Without this we cannot rely on waitpid() to tell 	 * what happened to our children. 	 */
name|signal
argument_list|(
name|SIGCHLD
argument_list|,
name|SIG_DFL
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|3
condition|)
name|usage
argument_list|(
literal|"git merge-index [-o] [-q]<merge-program> (-a | [--] [<filename>...])"
argument_list|)
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
name|i
operator|=
literal|1
expr_stmt|;
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
literal|"-o"
argument_list|)
condition|)
block|{
name|one_shot
operator|=
literal|1
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
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
literal|"-q"
argument_list|)
condition|)
block|{
name|quiet
operator|=
literal|1
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
name|pgm
operator|=
name|argv
index|[
name|i
operator|++
index|]
expr_stmt|;
for|for
control|(
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
name|force_file
operator|&&
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|force_file
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
literal|"-a"
argument_list|)
condition|)
block|{
name|merge_all
argument_list|()
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"git merge-index: unknown option %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
block|}
name|merge_one_path
argument_list|(
name|arg
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|err
operator|&&
operator|!
name|quiet
condition|)
name|die
argument_list|(
literal|"merge program failed"
argument_list|)
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
end_unit
