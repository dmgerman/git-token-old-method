begin_unit
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
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
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|"pull.h"
end_include
begin_decl_stmt
DECL|variable|use_link
specifier|static
name|int
name|use_link
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|use_symlink
specifier|static
name|int
name|use_symlink
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose
specifier|static
name|int
name|verbose
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|path
specifier|static
name|char
modifier|*
name|path
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|say
specifier|static
name|void
name|say
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
name|verbose
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
DECL|function|fetch
name|int
name|fetch
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|int
name|object_name_start
init|=
operator|-
literal|1
decl_stmt|;
specifier|static
name|char
name|filename
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|char
modifier|*
name|hex
init|=
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|dest_filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|int
name|ifd
decl_stmt|,
name|ofd
decl_stmt|,
name|status
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|;
if|if
condition|(
name|object_name_start
operator|<
literal|0
condition|)
block|{
name|strcpy
argument_list|(
name|filename
argument_list|,
name|path
argument_list|)
expr_stmt|;
comment|/* e.g. git.git */
name|strcat
argument_list|(
name|filename
argument_list|,
literal|"/objects/"
argument_list|)
expr_stmt|;
name|object_name_start
operator|=
name|strlen
argument_list|(
name|filename
argument_list|)
expr_stmt|;
block|}
name|filename
index|[
name|object_name_start
operator|+
literal|0
index|]
operator|=
name|hex
index|[
literal|0
index|]
expr_stmt|;
name|filename
index|[
name|object_name_start
operator|+
literal|1
index|]
operator|=
name|hex
index|[
literal|1
index|]
expr_stmt|;
name|filename
index|[
name|object_name_start
operator|+
literal|2
index|]
operator|=
literal|'/'
expr_stmt|;
name|strcpy
argument_list|(
name|filename
operator|+
name|object_name_start
operator|+
literal|3
argument_list|,
name|hex
operator|+
literal|2
argument_list|)
expr_stmt|;
if|if
condition|(
name|use_link
operator|&&
operator|!
name|link
argument_list|(
name|filename
argument_list|,
name|dest_filename
argument_list|)
condition|)
block|{
name|say
argument_list|(
literal|"Hardlinked %s.\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|use_symlink
operator|&&
operator|!
name|symlink
argument_list|(
name|filename
argument_list|,
name|dest_filename
argument_list|)
condition|)
block|{
name|say
argument_list|(
literal|"Symlinked %s.\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|ifd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|ifd
operator|<
literal|0
operator|||
name|fstat
argument_list|(
name|ifd
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Cannot open %s\n"
argument_list|,
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|ifd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
if|if
condition|(
operator|-
literal|1
operator|==
operator|(
name|int
operator|)
operator|(
name|long
operator|)
name|map
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Cannot mmap %s\n"
argument_list|,
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|ofd
operator|=
name|open
argument_list|(
name|dest_filename
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
name|status
operator|=
operator|(
operator|(
name|ofd
operator|<
literal|0
operator|)
operator|||
operator|(
name|write
argument_list|(
name|ofd
argument_list|,
name|map
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
operator|!=
name|st
operator|.
name|st_size
operator|)
operator|)
expr_stmt|;
name|munmap
argument_list|(
name|map
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ofd
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Cannot write %s (%ld bytes)\n"
argument_list|,
name|dest_filename
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
expr_stmt|;
else|else
name|say
argument_list|(
literal|"Copied %s.\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
name|status
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|local_pull_usage
specifier|static
specifier|const
name|char
modifier|*
name|local_pull_usage
init|=
literal|"git-local-pull [-c] [-t] [-a] [-l] [-s] [-v] commit-id path"
decl_stmt|;
end_decl_stmt
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
name|char
modifier|*
name|commit_id
decl_stmt|;
name|int
name|arg
init|=
literal|1
decl_stmt|;
while|while
condition|(
name|arg
operator|<
name|argc
operator|&&
name|argv
index|[
name|arg
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'t'
condition|)
name|get_tree
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'c'
condition|)
name|get_history
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'a'
condition|)
block|{
name|get_all
operator|=
literal|1
expr_stmt|;
name|get_tree
operator|=
literal|1
expr_stmt|;
name|get_history
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'l'
condition|)
name|use_link
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'s'
condition|)
name|use_symlink
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'v'
condition|)
name|verbose
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|local_pull_usage
argument_list|)
expr_stmt|;
name|arg
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|<
name|arg
operator|+
literal|2
condition|)
name|usage
argument_list|(
name|local_pull_usage
argument_list|)
expr_stmt|;
name|commit_id
operator|=
name|argv
index|[
name|arg
index|]
expr_stmt|;
name|path
operator|=
name|argv
index|[
name|arg
operator|+
literal|1
index|]
expr_stmt|;
if|if
condition|(
name|pull
argument_list|(
name|commit_id
argument_list|)
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
