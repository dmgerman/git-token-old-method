begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_function
DECL|function|contains
specifier|static
name|int
name|contains
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|one
parameter_list|,
specifier|const
name|char
modifier|*
name|needle
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|)
block|{
name|unsigned
name|long
name|offset
decl_stmt|,
name|sz
decl_stmt|;
specifier|const
name|char
modifier|*
name|data
decl_stmt|;
if|if
condition|(
name|diff_populate_filespec
argument_list|(
name|one
argument_list|)
condition|)
return|return
literal|0
return|;
name|sz
operator|=
name|one
operator|->
name|size
expr_stmt|;
name|data
operator|=
name|one
operator|->
name|data
expr_stmt|;
for|for
control|(
name|offset
operator|=
literal|0
init|;
name|offset
operator|+
name|len
operator|<=
name|sz
condition|;
name|offset
operator|++
control|)
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|needle
argument_list|,
name|data
operator|+
name|offset
argument_list|,
name|len
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
begin_function
DECL|function|diffcore_pickaxe
name|void
name|diffcore_pickaxe
parameter_list|(
specifier|const
name|char
modifier|*
name|needle
parameter_list|)
block|{
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|unsigned
name|long
name|len
init|=
name|strlen
argument_list|(
name|needle
argument_list|)
decl_stmt|;
name|int
name|i
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
name|outq
operator|.
name|queue
operator|=
name|NULL
expr_stmt|;
name|outq
operator|.
name|nr
operator|=
name|outq
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
name|int
name|onum
init|=
name|outq
operator|.
name|nr
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
continue|continue;
comment|/* ignore nonsense */
comment|/* created */
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|two
argument_list|,
name|needle
argument_list|,
name|len
argument_list|)
condition|)
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|needle
argument_list|,
name|len
argument_list|)
condition|)
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|needle
argument_list|,
name|len
argument_list|)
operator|!=
name|contains
argument_list|(
name|p
operator|->
name|two
argument_list|,
name|needle
argument_list|,
name|len
argument_list|)
condition|)
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
name|onum
operator|==
name|outq
operator|.
name|nr
condition|)
name|diff_free_filepair
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|q
operator|->
name|queue
argument_list|)
expr_stmt|;
operator|*
name|q
operator|=
name|outq
expr_stmt|;
return|return;
block|}
end_function
end_unit
