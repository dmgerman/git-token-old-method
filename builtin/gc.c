begin_unit
begin_comment
comment|/*  * git gc builtin command  *  * Cleanup unreachable files and optimize the repository.  *  * Copyright (c) 2007 James Bowes  *  * Based on git-gc.sh, which is  *  * Copyright (c) 2006 Shawn O. Pearce  */
end_comment
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"lockfile.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_define
DECL|macro|FAILED_RUN
define|#
directive|define
name|FAILED_RUN
value|"failed to run %s"
end_define
begin_decl_stmt
DECL|variable|builtin_gc_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_gc_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git gc [options]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pack_refs
specifier|static
name|int
name|pack_refs
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prune_reflogs
specifier|static
name|int
name|prune_reflogs
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|aggressive_depth
specifier|static
name|int
name|aggressive_depth
init|=
literal|250
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|aggressive_window
specifier|static
name|int
name|aggressive_window
init|=
literal|250
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|gc_auto_threshold
specifier|static
name|int
name|gc_auto_threshold
init|=
literal|6700
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|gc_auto_pack_limit
specifier|static
name|int
name|gc_auto_pack_limit
init|=
literal|50
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|detach_auto
specifier|static
name|int
name|detach_auto
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prune_expire
specifier|static
specifier|const
name|char
modifier|*
name|prune_expire
init|=
literal|"2.weeks.ago"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prune_worktrees_expire
specifier|static
specifier|const
name|char
modifier|*
name|prune_worktrees_expire
init|=
literal|"3.months.ago"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pack_refs_cmd
specifier|static
name|struct
name|argv_array
name|pack_refs_cmd
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|reflog
specifier|static
name|struct
name|argv_array
name|reflog
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|repack
specifier|static
name|struct
name|argv_array
name|repack
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prune
specifier|static
name|struct
name|argv_array
name|prune
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prune_worktrees
specifier|static
name|struct
name|argv_array
name|prune_worktrees
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|rerere
specifier|static
name|struct
name|argv_array
name|rerere
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pidfile
specifier|static
name|char
modifier|*
name|pidfile
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|remove_pidfile
specifier|static
name|void
name|remove_pidfile
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|pidfile
condition|)
name|unlink
argument_list|(
name|pidfile
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_pidfile_on_signal
specifier|static
name|void
name|remove_pidfile_on_signal
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
name|remove_pidfile
argument_list|()
expr_stmt|;
name|sigchain_pop
argument_list|(
name|signo
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|signo
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_config_date_string
specifier|static
name|void
name|git_config_date_string
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|output
parameter_list|)
block|{
if|if
condition|(
name|git_config_get_string_const
argument_list|(
name|key
argument_list|,
name|output
argument_list|)
condition|)
return|return;
if|if
condition|(
name|strcmp
argument_list|(
operator|*
name|output
argument_list|,
literal|"now"
argument_list|)
condition|)
block|{
name|unsigned
name|long
name|now
init|=
name|approxidate
argument_list|(
literal|"now"
argument_list|)
decl_stmt|;
if|if
condition|(
name|approxidate
argument_list|(
operator|*
name|output
argument_list|)
operator|>=
name|now
condition|)
name|git_die_config
argument_list|(
name|key
argument_list|,
name|_
argument_list|(
literal|"Invalid %s: '%s'"
argument_list|)
argument_list|,
name|key
argument_list|,
operator|*
name|output
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|gc_config
specifier|static
name|void
name|gc_config
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|value
decl_stmt|;
if|if
condition|(
operator|!
name|git_config_get_value
argument_list|(
literal|"gc.packrefs"
argument_list|,
operator|&
name|value
argument_list|)
condition|)
block|{
if|if
condition|(
name|value
operator|&&
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"notbare"
argument_list|)
condition|)
name|pack_refs
operator|=
operator|-
literal|1
expr_stmt|;
else|else
name|pack_refs
operator|=
name|git_config_bool
argument_list|(
literal|"gc.packrefs"
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
name|git_config_get_int
argument_list|(
literal|"gc.aggressivewindow"
argument_list|,
operator|&
name|aggressive_window
argument_list|)
expr_stmt|;
name|git_config_get_int
argument_list|(
literal|"gc.aggressivedepth"
argument_list|,
operator|&
name|aggressive_depth
argument_list|)
expr_stmt|;
name|git_config_get_int
argument_list|(
literal|"gc.auto"
argument_list|,
operator|&
name|gc_auto_threshold
argument_list|)
expr_stmt|;
name|git_config_get_int
argument_list|(
literal|"gc.autopacklimit"
argument_list|,
operator|&
name|gc_auto_pack_limit
argument_list|)
expr_stmt|;
name|git_config_get_bool
argument_list|(
literal|"gc.autodetach"
argument_list|,
operator|&
name|detach_auto
argument_list|)
expr_stmt|;
name|git_config_date_string
argument_list|(
literal|"gc.pruneexpire"
argument_list|,
operator|&
name|prune_expire
argument_list|)
expr_stmt|;
name|git_config_date_string
argument_list|(
literal|"gc.pruneworktreesexpire"
argument_list|,
operator|&
name|prune_worktrees_expire
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|too_many_loose_objects
specifier|static
name|int
name|too_many_loose_objects
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* 	 * Quickly check if a "gc" is needed, by estimating how 	 * many loose objects there are.  Because SHA-1 is evenly 	 * distributed, we can check only one and get a reasonable 	 * estimate. 	 */
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|objdir
init|=
name|get_object_directory
argument_list|()
decl_stmt|;
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|ent
decl_stmt|;
name|int
name|auto_threshold
decl_stmt|;
name|int
name|num_loose
init|=
literal|0
decl_stmt|;
name|int
name|needed
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|gc_auto_threshold
operator|<=
literal|0
condition|)
return|return
literal|0
return|;
if|if
condition|(
sizeof|sizeof
argument_list|(
name|path
argument_list|)
operator|<=
name|snprintf
argument_list|(
name|path
argument_list|,
sizeof|sizeof
argument_list|(
name|path
argument_list|)
argument_list|,
literal|"%s/17"
argument_list|,
name|objdir
argument_list|)
condition|)
block|{
name|warning
argument_list|(
name|_
argument_list|(
literal|"insanely long object directory %.*s"
argument_list|)
argument_list|,
literal|50
argument_list|,
name|objdir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|dir
operator|=
name|opendir
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return
literal|0
return|;
name|auto_threshold
operator|=
operator|(
name|gc_auto_threshold
operator|+
literal|255
operator|)
operator|/
literal|256
expr_stmt|;
while|while
condition|(
operator|(
name|ent
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
name|strspn
argument_list|(
name|ent
operator|->
name|d_name
argument_list|,
literal|"0123456789abcdef"
argument_list|)
operator|!=
literal|38
operator|||
name|ent
operator|->
name|d_name
index|[
literal|38
index|]
operator|!=
literal|'\0'
condition|)
continue|continue;
if|if
condition|(
operator|++
name|num_loose
operator|>
name|auto_threshold
condition|)
block|{
name|needed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
name|needed
return|;
block|}
end_function
begin_function
DECL|function|too_many_packs
specifier|static
name|int
name|too_many_packs
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
name|int
name|cnt
decl_stmt|;
if|if
condition|(
name|gc_auto_pack_limit
operator|<=
literal|0
condition|)
return|return
literal|0
return|;
name|prepare_packed_git
argument_list|()
expr_stmt|;
for|for
control|(
name|cnt
operator|=
literal|0
operator|,
name|p
operator|=
name|packed_git
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|p
operator|->
name|pack_local
condition|)
continue|continue;
if|if
condition|(
name|p
operator|->
name|pack_keep
condition|)
continue|continue;
comment|/* 		 * Perhaps check the size of the pack and count only 		 * very small ones here? 		 */
name|cnt
operator|++
expr_stmt|;
block|}
return|return
name|gc_auto_pack_limit
operator|<=
name|cnt
return|;
block|}
end_function
begin_function
DECL|function|add_repack_all_option
specifier|static
name|void
name|add_repack_all_option
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|prune_expire
operator|&&
operator|!
name|strcmp
argument_list|(
name|prune_expire
argument_list|,
literal|"now"
argument_list|)
condition|)
name|argv_array_push
argument_list|(
operator|&
name|repack
argument_list|,
literal|"-a"
argument_list|)
expr_stmt|;
else|else
block|{
name|argv_array_push
argument_list|(
operator|&
name|repack
argument_list|,
literal|"-A"
argument_list|)
expr_stmt|;
if|if
condition|(
name|prune_expire
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|repack
argument_list|,
literal|"--unpack-unreachable=%s"
argument_list|,
name|prune_expire
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|need_to_gc
specifier|static
name|int
name|need_to_gc
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* 	 * Setting gc.auto to 0 or negative can disable the 	 * automatic gc. 	 */
if|if
condition|(
name|gc_auto_threshold
operator|<=
literal|0
condition|)
return|return
literal|0
return|;
comment|/* 	 * If there are too many loose objects, but not too many 	 * packs, we run "repack -d -l".  If there are too many packs, 	 * we run "repack -A -d -l".  Otherwise we tell the caller 	 * there is no need. 	 */
if|if
condition|(
name|too_many_packs
argument_list|()
condition|)
name|add_repack_all_option
argument_list|()
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|too_many_loose_objects
argument_list|()
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|run_hook_le
argument_list|(
name|NULL
argument_list|,
literal|"pre-auto-gc"
argument_list|,
name|NULL
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/* return NULL on success, else hostname running the gc */
end_comment
begin_function
DECL|function|lock_repo_for_gc
specifier|static
specifier|const
name|char
modifier|*
name|lock_repo_for_gc
parameter_list|(
name|int
name|force
parameter_list|,
name|pid_t
modifier|*
name|ret_pid
parameter_list|)
block|{
specifier|static
name|struct
name|lock_file
name|lock
decl_stmt|;
name|char
name|my_host
index|[
literal|128
index|]
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|uintmax_t
name|pid
decl_stmt|;
name|FILE
modifier|*
name|fp
decl_stmt|;
name|int
name|fd
decl_stmt|;
if|if
condition|(
name|pidfile
condition|)
comment|/* already locked */
return|return
name|NULL
return|;
if|if
condition|(
name|gethostname
argument_list|(
name|my_host
argument_list|,
sizeof|sizeof
argument_list|(
name|my_host
argument_list|)
argument_list|)
condition|)
name|strcpy
argument_list|(
name|my_host
argument_list|,
literal|"unknown"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|hold_lock_file_for_update
argument_list|(
operator|&
name|lock
argument_list|,
name|git_path
argument_list|(
literal|"gc.pid"
argument_list|)
argument_list|,
name|LOCK_DIE_ON_ERROR
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|force
condition|)
block|{
specifier|static
name|char
name|locking_host
index|[
literal|128
index|]
decl_stmt|;
name|int
name|should_exit
decl_stmt|;
name|fp
operator|=
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"gc.pid"
argument_list|)
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|locking_host
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|locking_host
argument_list|)
argument_list|)
expr_stmt|;
name|should_exit
operator|=
name|fp
operator|!=
name|NULL
operator|&&
operator|!
name|fstat
argument_list|(
name|fileno
argument_list|(
name|fp
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
operator|&&
comment|/* 			 * 12 hour limit is very generous as gc should 			 * never take that long. On the other hand we 			 * don't really need a strict limit here, 			 * running gc --auto one day late is not a big 			 * problem. --force can be used in manual gc 			 * after the user verifies that no gc is 			 * running. 			 */
name|time
argument_list|(
name|NULL
argument_list|)
operator|-
name|st
operator|.
name|st_mtime
operator|<=
literal|12
operator|*
literal|3600
operator|&&
name|fscanf
argument_list|(
name|fp
argument_list|,
literal|"%"
name|PRIuMAX
literal|" %127c"
argument_list|,
operator|&
name|pid
argument_list|,
name|locking_host
argument_list|)
operator|==
literal|2
operator|&&
comment|/* be gentle to concurrent "gc" on remote hosts */
operator|(
name|strcmp
argument_list|(
name|locking_host
argument_list|,
name|my_host
argument_list|)
operator|||
operator|!
name|kill
argument_list|(
name|pid
argument_list|,
literal|0
argument_list|)
operator|||
name|errno
operator|==
name|EPERM
operator|)
expr_stmt|;
if|if
condition|(
name|fp
operator|!=
name|NULL
condition|)
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
if|if
condition|(
name|should_exit
condition|)
block|{
if|if
condition|(
name|fd
operator|>=
literal|0
condition|)
name|rollback_lock_file
argument_list|(
operator|&
name|lock
argument_list|)
expr_stmt|;
operator|*
name|ret_pid
operator|=
name|pid
expr_stmt|;
return|return
name|locking_host
return|;
block|}
block|}
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%"
name|PRIuMAX
literal|" %s"
argument_list|,
operator|(
name|uintmax_t
operator|)
name|getpid
argument_list|()
argument_list|,
name|my_host
argument_list|)
expr_stmt|;
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|sb
operator|.
name|buf
argument_list|,
name|sb
operator|.
name|len
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|commit_lock_file
argument_list|(
operator|&
name|lock
argument_list|)
expr_stmt|;
name|pidfile
operator|=
name|git_pathdup
argument_list|(
literal|"gc.pid"
argument_list|)
expr_stmt|;
name|sigchain_push_common
argument_list|(
name|remove_pidfile_on_signal
argument_list|)
expr_stmt|;
name|atexit
argument_list|(
name|remove_pidfile
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|gc_before_repack
specifier|static
name|int
name|gc_before_repack
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|pack_refs
operator|&&
name|run_command_v_opt
argument_list|(
name|pack_refs_cmd
operator|.
name|argv
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|pack_refs_cmd
operator|.
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|prune_reflogs
operator|&&
name|run_command_v_opt
argument_list|(
name|reflog
operator|.
name|argv
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|reflog
operator|.
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
name|pack_refs
operator|=
literal|0
expr_stmt|;
name|prune_reflogs
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_gc
name|int
name|cmd_gc
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
name|aggressive
init|=
literal|0
decl_stmt|;
name|int
name|auto_gc
init|=
literal|0
decl_stmt|;
name|int
name|quiet
init|=
literal|0
decl_stmt|;
name|int
name|force
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|struct
name|option
name|builtin_gc_options
index|[]
init|=
block|{
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
name|N_
argument_list|(
literal|"suppress progress reporting"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_STRING
block|,
literal|0
block|,
literal|"prune"
block|,
operator|&
name|prune_expire
block|,
name|N_
argument_list|(
literal|"date"
argument_list|)
block|,
name|N_
argument_list|(
literal|"prune unreferenced objects"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
block|,
name|NULL
block|,
operator|(
name|intptr_t
operator|)
name|prune_expire
block|}
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"aggressive"
argument_list|,
operator|&
name|aggressive
argument_list|,
name|N_
argument_list|(
literal|"be more thorough (increased runtime)"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"auto"
argument_list|,
operator|&
name|auto_gc
argument_list|,
name|N_
argument_list|(
literal|"enable auto-gc mode"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"force"
argument_list|,
operator|&
name|force
argument_list|,
name|N_
argument_list|(
literal|"force running gc even if there may be another gc running"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
if|if
condition|(
name|argc
operator|==
literal|2
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
condition|)
name|usage_with_options
argument_list|(
name|builtin_gc_usage
argument_list|,
name|builtin_gc_options
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|pack_refs_cmd
argument_list|,
literal|"pack-refs"
argument_list|,
literal|"--all"
argument_list|,
literal|"--prune"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|reflog
argument_list|,
literal|"reflog"
argument_list|,
literal|"expire"
argument_list|,
literal|"--all"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|repack
argument_list|,
literal|"repack"
argument_list|,
literal|"-d"
argument_list|,
literal|"-l"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|prune
argument_list|,
literal|"prune"
argument_list|,
literal|"--expire"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|prune_worktrees
argument_list|,
literal|"worktree"
argument_list|,
literal|"prune"
argument_list|,
literal|"--expire"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|rerere
argument_list|,
literal|"rerere"
argument_list|,
literal|"gc"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|gc_config
argument_list|()
expr_stmt|;
if|if
condition|(
name|pack_refs
operator|<
literal|0
condition|)
name|pack_refs
operator|=
operator|!
name|is_bare_repository
argument_list|()
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|builtin_gc_options
argument_list|,
name|builtin_gc_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|0
condition|)
name|usage_with_options
argument_list|(
name|builtin_gc_usage
argument_list|,
name|builtin_gc_options
argument_list|)
expr_stmt|;
if|if
condition|(
name|aggressive
condition|)
block|{
name|argv_array_push
argument_list|(
operator|&
name|repack
argument_list|,
literal|"-f"
argument_list|)
expr_stmt|;
if|if
condition|(
name|aggressive_depth
operator|>
literal|0
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|repack
argument_list|,
literal|"--depth=%d"
argument_list|,
name|aggressive_depth
argument_list|)
expr_stmt|;
if|if
condition|(
name|aggressive_window
operator|>
literal|0
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|repack
argument_list|,
literal|"--window=%d"
argument_list|,
name|aggressive_window
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|quiet
condition|)
name|argv_array_push
argument_list|(
operator|&
name|repack
argument_list|,
literal|"-q"
argument_list|)
expr_stmt|;
if|if
condition|(
name|auto_gc
condition|)
block|{
comment|/* 		 * Auto-gc should be least intrusive as possible. 		 */
if|if
condition|(
operator|!
name|need_to_gc
argument_list|()
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|quiet
condition|)
block|{
if|if
condition|(
name|detach_auto
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"Auto packing the repository in background for optimum performance.\n"
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"Auto packing the repository for optimum performance.\n"
argument_list|)
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"See \"git help gc\" for manual housekeeping.\n"
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|detach_auto
condition|)
block|{
if|if
condition|(
name|gc_before_repack
argument_list|()
condition|)
return|return
operator|-
literal|1
return|;
comment|/* 			 * failure to daemonize is ok, we'll continue 			 * in foreground 			 */
name|daemonize
argument_list|()
expr_stmt|;
block|}
block|}
else|else
name|add_repack_all_option
argument_list|()
expr_stmt|;
name|name
operator|=
name|lock_repo_for_gc
argument_list|(
name|force
argument_list|,
operator|&
name|pid
argument_list|)
expr_stmt|;
if|if
condition|(
name|name
condition|)
block|{
if|if
condition|(
name|auto_gc
condition|)
return|return
literal|0
return|;
comment|/* be quiet on --auto */
name|die
argument_list|(
name|_
argument_list|(
literal|"gc is already running on machine '%s' pid %"
name|PRIuMAX
literal|" (use --force if not)"
argument_list|)
argument_list|,
name|name
argument_list|,
operator|(
name|uintmax_t
operator|)
name|pid
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|gc_before_repack
argument_list|()
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|repack
operator|.
name|argv
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|repack
operator|.
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|prune_expire
condition|)
block|{
name|argv_array_push
argument_list|(
operator|&
name|prune
argument_list|,
name|prune_expire
argument_list|)
expr_stmt|;
if|if
condition|(
name|quiet
condition|)
name|argv_array_push
argument_list|(
operator|&
name|prune
argument_list|,
literal|"--no-progress"
argument_list|)
expr_stmt|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|prune
operator|.
name|argv
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|prune
operator|.
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
block|}
if|if
condition|(
name|prune_worktrees_expire
condition|)
block|{
name|argv_array_push
argument_list|(
operator|&
name|prune_worktrees
argument_list|,
name|prune_worktrees_expire
argument_list|)
expr_stmt|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|prune_worktrees
operator|.
name|argv
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|prune_worktrees
operator|.
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
block|}
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|rerere
operator|.
name|argv
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|rerere
operator|.
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|auto_gc
operator|&&
name|too_many_loose_objects
argument_list|()
condition|)
name|warning
argument_list|(
name|_
argument_list|(
literal|"There are too many unreachable loose objects; "
literal|"run 'git prune' to remove them."
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
