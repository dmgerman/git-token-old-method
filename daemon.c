begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|<signal.h>
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
end_include
begin_include
include|#
directive|include
file|<sys/socket.h>
end_include
begin_include
include|#
directive|include
file|<sys/time.h>
end_include
begin_include
include|#
directive|include
file|<sys/poll.h>
end_include
begin_include
include|#
directive|include
file|<netdb.h>
end_include
begin_include
include|#
directive|include
file|<netinet/in.h>
end_include
begin_include
include|#
directive|include
file|<arpa/inet.h>
end_include
begin_include
include|#
directive|include
file|<syslog.h>
end_include
begin_decl_stmt
DECL|variable|log_syslog
specifier|static
name|int
name|log_syslog
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose
specifier|static
name|int
name|verbose
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|daemon_usage
specifier|static
specifier|const
name|char
name|daemon_usage
index|[]
init|=
literal|"git-daemon [--verbose] [--syslog] [--inetd | --port=n] [--export-all]\n"
literal|"           [--timeout=n] [--init-timeout=n] [directory...]"
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* List of acceptable pathname prefixes */
end_comment
begin_decl_stmt
DECL|variable|ok_paths
specifier|static
name|char
modifier|*
modifier|*
name|ok_paths
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* If this is set, git-daemon-export-ok is not required */
end_comment
begin_decl_stmt
DECL|variable|export_all_trees
specifier|static
name|int
name|export_all_trees
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* Timeout, and initial timeout */
end_comment
begin_decl_stmt
DECL|variable|timeout
specifier|static
name|unsigned
name|int
name|timeout
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|init_timeout
specifier|static
name|unsigned
name|int
name|init_timeout
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|logreport
specifier|static
name|void
name|logreport
parameter_list|(
name|int
name|priority
parameter_list|,
specifier|const
name|char
modifier|*
name|err
parameter_list|,
name|va_list
name|params
parameter_list|)
block|{
comment|/* We should do a single write so that it is atomic and output 	 * of several processes do not get intermingled. */
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
name|int
name|buflen
decl_stmt|;
name|int
name|maxlen
decl_stmt|,
name|msglen
decl_stmt|;
comment|/* sizeof(buf) should be big enough for "[pid] \n" */
name|buflen
operator|=
name|snprintf
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
literal|"[%ld] "
argument_list|,
operator|(
name|long
operator|)
name|getpid
argument_list|()
argument_list|)
expr_stmt|;
name|maxlen
operator|=
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
operator|-
name|buflen
operator|-
literal|1
expr_stmt|;
comment|/* -1 for our own LF */
name|msglen
operator|=
name|vsnprintf
argument_list|(
name|buf
operator|+
name|buflen
argument_list|,
name|maxlen
argument_list|,
name|err
argument_list|,
name|params
argument_list|)
expr_stmt|;
if|if
condition|(
name|log_syslog
condition|)
block|{
name|syslog
argument_list|(
name|priority
argument_list|,
literal|"%s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
return|return;
block|}
comment|/* maxlen counted our own LF but also counts space given to 	 * vsnprintf for the terminating NUL.  We want to make sure that 	 * we have space for our own LF and NUL after the "meat" of the 	 * message, so truncate it at maxlen - 1. 	 */
if|if
condition|(
name|msglen
operator|>
name|maxlen
operator|-
literal|1
condition|)
name|msglen
operator|=
name|maxlen
operator|-
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|msglen
operator|<
literal|0
condition|)
name|msglen
operator|=
literal|0
expr_stmt|;
comment|/* Protect against weird return values. */
name|buflen
operator|+=
name|msglen
expr_stmt|;
name|buf
index|[
name|buflen
operator|++
index|]
operator|=
literal|'\n'
expr_stmt|;
name|buf
index|[
name|buflen
index|]
operator|=
literal|'\0'
expr_stmt|;
name|write
argument_list|(
literal|2
argument_list|,
name|buf
argument_list|,
name|buflen
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|logerror
specifier|static
name|void
name|logerror
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|params
decl_stmt|;
name|va_start
argument_list|(
name|params
argument_list|,
name|err
argument_list|)
expr_stmt|;
name|logreport
argument_list|(
name|LOG_ERR
argument_list|,
name|err
argument_list|,
name|params
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|params
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|loginfo
specifier|static
name|void
name|loginfo
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|params
decl_stmt|;
if|if
condition|(
operator|!
name|verbose
condition|)
return|return;
name|va_start
argument_list|(
name|params
argument_list|,
name|err
argument_list|)
expr_stmt|;
name|logreport
argument_list|(
name|LOG_INFO
argument_list|,
name|err
argument_list|,
name|params
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|params
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|path_ok
specifier|static
name|int
name|path_ok
parameter_list|(
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|dir
decl_stmt|;
name|char
modifier|*
modifier|*
name|pp
decl_stmt|;
name|int
name|sl
decl_stmt|,
name|ndot
decl_stmt|;
comment|/* The pathname here should be an absolute path. */
if|if
condition|(
operator|*
name|p
operator|++
operator|!=
literal|'/'
condition|)
return|return
literal|0
return|;
name|sl
operator|=
literal|1
expr_stmt|;
name|ndot
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
operator|*
name|p
operator|==
literal|'.'
condition|)
block|{
name|ndot
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|*
name|p
operator|==
literal|'\0'
condition|)
block|{
comment|/* Reject "." and ".." at the end of the path */
if|if
condition|(
name|sl
operator|&&
name|ndot
operator|>
literal|0
operator|&&
name|ndot
operator|<
literal|3
condition|)
return|return
literal|0
return|;
comment|/* Otherwise OK */
break|break;
block|}
elseif|else
if|if
condition|(
operator|*
name|p
operator|==
literal|'/'
condition|)
block|{
comment|/* Refuse "", "." or ".." */
if|if
condition|(
name|sl
operator|&&
name|ndot
operator|<
literal|3
condition|)
return|return
literal|0
return|;
name|sl
operator|=
literal|1
expr_stmt|;
name|ndot
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
name|sl
operator|=
name|ndot
operator|=
literal|0
expr_stmt|;
block|}
name|p
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|ok_paths
operator|&&
operator|*
name|ok_paths
condition|)
block|{
name|int
name|ok
init|=
literal|0
decl_stmt|;
name|int
name|dirlen
init|=
name|strlen
argument_list|(
name|dir
argument_list|)
decl_stmt|;
for|for
control|(
name|pp
operator|=
name|ok_paths
init|;
operator|*
name|pp
condition|;
name|pp
operator|++
control|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
operator|*
name|pp
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<=
name|dirlen
operator|&&
operator|!
name|strncmp
argument_list|(
operator|*
name|pp
argument_list|,
name|dir
argument_list|,
name|len
argument_list|)
operator|&&
operator|(
name|dir
index|[
name|len
index|]
operator|==
literal|'/'
operator|||
name|dir
index|[
name|len
index|]
operator|==
literal|'\0'
operator|)
condition|)
block|{
name|ok
operator|=
literal|1
expr_stmt|;
break|break;
block|}
block|}
if|if
condition|(
operator|!
name|ok
condition|)
return|return
literal|0
return|;
comment|/* Path not in whitelist */
block|}
return|return
literal|1
return|;
comment|/* Path acceptable */
block|}
end_function
begin_function
DECL|function|set_dir
specifier|static
name|int
name|set_dir
parameter_list|(
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
block|{
if|if
condition|(
operator|!
name|path_ok
argument_list|(
name|dir
argument_list|)
condition|)
block|{
name|errno
operator|=
name|EACCES
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|chdir
argument_list|(
name|dir
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
comment|/* 	 * Security on the cheap. 	 * 	 * We want a readable HEAD, usable "objects" directory, and  	 * a "git-daemon-export-ok" flag that says that the other side 	 * is ok with us doing this. 	 */
if|if
condition|(
operator|!
name|export_all_trees
operator|&&
name|access
argument_list|(
literal|"git-daemon-export-ok"
argument_list|,
name|F_OK
argument_list|)
condition|)
block|{
name|errno
operator|=
name|EACCES
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|access
argument_list|(
literal|"objects/"
argument_list|,
name|X_OK
argument_list|)
operator|||
name|access
argument_list|(
literal|"HEAD"
argument_list|,
name|R_OK
argument_list|)
condition|)
block|{
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* If all this passed, we're OK */
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|upload
specifier|static
name|int
name|upload
parameter_list|(
name|char
modifier|*
name|dir
parameter_list|)
block|{
comment|/* Try paths in this order */
specifier|static
specifier|const
name|char
modifier|*
name|paths
index|[]
init|=
block|{
literal|"%s"
block|,
literal|"%s/.git"
block|,
literal|"%s.git"
block|,
literal|"%s.git/.git"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|pp
decl_stmt|;
comment|/* Enough for the longest path above including final null */
name|int
name|buflen
init|=
name|strlen
argument_list|(
name|dir
argument_list|)
operator|+
literal|10
decl_stmt|;
name|char
modifier|*
name|dirbuf
init|=
name|xmalloc
argument_list|(
name|buflen
argument_list|)
decl_stmt|;
comment|/* Timeout as string */
name|char
name|timeout_buf
index|[
literal|64
index|]
decl_stmt|;
name|loginfo
argument_list|(
literal|"Request for '%s'"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
for|for
control|(
name|pp
operator|=
name|paths
init|;
operator|*
name|pp
condition|;
name|pp
operator|++
control|)
block|{
name|snprintf
argument_list|(
name|dirbuf
argument_list|,
name|buflen
argument_list|,
operator|*
name|pp
argument_list|,
name|dir
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|set_dir
argument_list|(
name|dirbuf
argument_list|)
condition|)
break|break;
block|}
if|if
condition|(
operator|!
operator|*
name|pp
condition|)
block|{
name|logerror
argument_list|(
literal|"Cannot set directory '%s': %s"
argument_list|,
name|dir
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* 	 * We'll ignore SIGTERM from now on, we have a 	 * good client. 	 */
name|signal
argument_list|(
name|SIGTERM
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
name|snprintf
argument_list|(
name|timeout_buf
argument_list|,
sizeof|sizeof
name|timeout_buf
argument_list|,
literal|"--timeout=%u"
argument_list|,
name|timeout
argument_list|)
expr_stmt|;
comment|/* git-upload-pack only ever reads stuff, so this is safe */
name|execlp
argument_list|(
literal|"git-upload-pack"
argument_list|,
literal|"git-upload-pack"
argument_list|,
literal|"--strict"
argument_list|,
name|timeout_buf
argument_list|,
literal|"."
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|execute
specifier|static
name|int
name|execute
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|int
name|len
decl_stmt|;
name|alarm
argument_list|(
name|init_timeout
condition|?
name|init_timeout
else|:
name|timeout
argument_list|)
expr_stmt|;
name|len
operator|=
name|packet_read_line
argument_list|(
literal|0
argument_list|,
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|)
expr_stmt|;
name|alarm
argument_list|(
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|line
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
literal|"git-upload-pack /"
argument_list|,
name|line
argument_list|,
literal|17
argument_list|)
condition|)
return|return
name|upload
argument_list|(
name|line
operator|+
literal|16
argument_list|)
return|;
name|logerror
argument_list|(
literal|"Protocol error: '%s'"
argument_list|,
name|line
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * We count spawned/reaped separately, just to avoid any  * races when updating them from signals. The SIGCHLD handler  * will only update children_reaped, and the fork logic will  * only update children_spawned.  *  * MAX_CHILDREN should be a power-of-two to make the modulus  * operation cheap. It should also be at least twice  * the maximum number of connections we will ever allow.  */
end_comment
begin_define
DECL|macro|MAX_CHILDREN
define|#
directive|define
name|MAX_CHILDREN
value|128
end_define
begin_decl_stmt
DECL|variable|max_connections
specifier|static
name|int
name|max_connections
init|=
literal|25
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* These are updated by the signal handler */
end_comment
begin_decl_stmt
DECL|variable|children_reaped
specifier|static
specifier|volatile
name|unsigned
name|int
name|children_reaped
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|dead_child
specifier|static
name|pid_t
name|dead_child
index|[
name|MAX_CHILDREN
index|]
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* These are updated by the main loop */
end_comment
begin_decl_stmt
DECL|variable|children_spawned
specifier|static
name|unsigned
name|int
name|children_spawned
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|children_deleted
specifier|static
name|unsigned
name|int
name|children_deleted
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|child
specifier|static
struct|struct
name|child
block|{
DECL|member|pid
name|pid_t
name|pid
decl_stmt|;
DECL|member|addrlen
name|int
name|addrlen
decl_stmt|;
DECL|member|address
name|struct
name|sockaddr_storage
name|address
decl_stmt|;
block|}
DECL|variable|live_child
name|live_child
index|[
name|MAX_CHILDREN
index|]
struct|;
end_struct
begin_function
DECL|function|add_child
specifier|static
name|void
name|add_child
parameter_list|(
name|int
name|idx
parameter_list|,
name|pid_t
name|pid
parameter_list|,
name|struct
name|sockaddr
modifier|*
name|addr
parameter_list|,
name|int
name|addrlen
parameter_list|)
block|{
name|live_child
index|[
name|idx
index|]
operator|.
name|pid
operator|=
name|pid
expr_stmt|;
name|live_child
index|[
name|idx
index|]
operator|.
name|addrlen
operator|=
name|addrlen
expr_stmt|;
name|memcpy
argument_list|(
operator|&
name|live_child
index|[
name|idx
index|]
operator|.
name|address
argument_list|,
name|addr
argument_list|,
name|addrlen
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Walk from "deleted" to "spawned", and remove child "pid".  *  * We move everything up by one, since the new "deleted" will  * be one higher.  */
end_comment
begin_function
DECL|function|remove_child
specifier|static
name|void
name|remove_child
parameter_list|(
name|pid_t
name|pid
parameter_list|,
name|unsigned
name|deleted
parameter_list|,
name|unsigned
name|spawned
parameter_list|)
block|{
name|struct
name|child
name|n
decl_stmt|;
name|deleted
operator|%=
name|MAX_CHILDREN
expr_stmt|;
name|spawned
operator|%=
name|MAX_CHILDREN
expr_stmt|;
if|if
condition|(
name|live_child
index|[
name|deleted
index|]
operator|.
name|pid
operator|==
name|pid
condition|)
block|{
name|live_child
index|[
name|deleted
index|]
operator|.
name|pid
operator|=
operator|-
literal|1
expr_stmt|;
return|return;
block|}
name|n
operator|=
name|live_child
index|[
name|deleted
index|]
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|child
name|m
decl_stmt|;
name|deleted
operator|=
operator|(
name|deleted
operator|+
literal|1
operator|)
operator|%
name|MAX_CHILDREN
expr_stmt|;
if|if
condition|(
name|deleted
operator|==
name|spawned
condition|)
name|die
argument_list|(
literal|"could not find dead child %d\n"
argument_list|,
name|pid
argument_list|)
expr_stmt|;
name|m
operator|=
name|live_child
index|[
name|deleted
index|]
expr_stmt|;
name|live_child
index|[
name|deleted
index|]
operator|=
name|n
expr_stmt|;
if|if
condition|(
name|m
operator|.
name|pid
operator|==
name|pid
condition|)
return|return;
name|n
operator|=
name|m
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * This gets called if the number of connections grows  * past "max_connections".  *  * We _should_ start off by searching for connections  * from the same IP, and if there is some address wth  * multiple connections, we should kill that first.  *  * As it is, we just "randomly" kill 25% of the connections,  * and our pseudo-random generator sucks too. I have no  * shame.  *  * Really, this is just a place-holder for a _real_ algorithm.  */
end_comment
begin_function
DECL|function|kill_some_children
specifier|static
name|void
name|kill_some_children
parameter_list|(
name|int
name|signo
parameter_list|,
name|unsigned
name|start
parameter_list|,
name|unsigned
name|stop
parameter_list|)
block|{
name|start
operator|%=
name|MAX_CHILDREN
expr_stmt|;
name|stop
operator|%=
name|MAX_CHILDREN
expr_stmt|;
while|while
condition|(
name|start
operator|!=
name|stop
condition|)
block|{
if|if
condition|(
operator|!
operator|(
name|start
operator|&
literal|3
operator|)
condition|)
name|kill
argument_list|(
name|live_child
index|[
name|start
index|]
operator|.
name|pid
argument_list|,
name|signo
argument_list|)
expr_stmt|;
name|start
operator|=
operator|(
name|start
operator|+
literal|1
operator|)
operator|%
name|MAX_CHILDREN
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|check_max_connections
specifier|static
name|void
name|check_max_connections
parameter_list|(
name|void
parameter_list|)
block|{
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|active
decl_stmt|;
name|unsigned
name|spawned
decl_stmt|,
name|reaped
decl_stmt|,
name|deleted
decl_stmt|;
name|spawned
operator|=
name|children_spawned
expr_stmt|;
name|reaped
operator|=
name|children_reaped
expr_stmt|;
name|deleted
operator|=
name|children_deleted
expr_stmt|;
while|while
condition|(
name|deleted
operator|<
name|reaped
condition|)
block|{
name|pid_t
name|pid
init|=
name|dead_child
index|[
name|deleted
operator|%
name|MAX_CHILDREN
index|]
decl_stmt|;
name|remove_child
argument_list|(
name|pid
argument_list|,
name|deleted
argument_list|,
name|spawned
argument_list|)
expr_stmt|;
name|deleted
operator|++
expr_stmt|;
block|}
name|children_deleted
operator|=
name|deleted
expr_stmt|;
name|active
operator|=
name|spawned
operator|-
name|deleted
expr_stmt|;
if|if
condition|(
name|active
operator|<=
name|max_connections
condition|)
break|break;
comment|/* Kill some unstarted connections with SIGTERM */
name|kill_some_children
argument_list|(
name|SIGTERM
argument_list|,
name|deleted
argument_list|,
name|spawned
argument_list|)
expr_stmt|;
if|if
condition|(
name|active
operator|<=
name|max_connections
operator|<<
literal|1
condition|)
break|break;
comment|/* If the SIGTERM thing isn't helping use SIGKILL */
name|kill_some_children
argument_list|(
name|SIGKILL
argument_list|,
name|deleted
argument_list|,
name|spawned
argument_list|)
expr_stmt|;
name|sleep
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|handle
specifier|static
name|void
name|handle
parameter_list|(
name|int
name|incoming
parameter_list|,
name|struct
name|sockaddr
modifier|*
name|addr
parameter_list|,
name|int
name|addrlen
parameter_list|)
block|{
name|pid_t
name|pid
init|=
name|fork
argument_list|()
decl_stmt|;
name|char
name|addrbuf
index|[
literal|256
index|]
init|=
literal|""
decl_stmt|;
name|int
name|port
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|pid
condition|)
block|{
name|unsigned
name|idx
decl_stmt|;
name|close
argument_list|(
name|incoming
argument_list|)
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
return|return;
name|idx
operator|=
name|children_spawned
operator|%
name|MAX_CHILDREN
expr_stmt|;
name|children_spawned
operator|++
expr_stmt|;
name|add_child
argument_list|(
name|idx
argument_list|,
name|pid
argument_list|,
name|addr
argument_list|,
name|addrlen
argument_list|)
expr_stmt|;
name|check_max_connections
argument_list|()
expr_stmt|;
return|return;
block|}
name|dup2
argument_list|(
name|incoming
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|incoming
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|incoming
argument_list|)
expr_stmt|;
if|if
condition|(
name|addr
operator|->
name|sa_family
operator|==
name|AF_INET
condition|)
block|{
name|struct
name|sockaddr_in
modifier|*
name|sin_addr
init|=
operator|(
name|void
operator|*
operator|)
name|addr
decl_stmt|;
name|inet_ntop
argument_list|(
name|AF_INET
argument_list|,
operator|&
name|sin_addr
operator|->
name|sin_addr
argument_list|,
name|addrbuf
argument_list|,
sizeof|sizeof
argument_list|(
name|addrbuf
argument_list|)
argument_list|)
expr_stmt|;
name|port
operator|=
name|sin_addr
operator|->
name|sin_port
expr_stmt|;
ifndef|#
directive|ifndef
name|NO_IPV6
block|}
elseif|else
if|if
condition|(
name|addr
operator|->
name|sa_family
operator|==
name|AF_INET6
condition|)
block|{
name|struct
name|sockaddr_in6
modifier|*
name|sin6_addr
init|=
operator|(
name|void
operator|*
operator|)
name|addr
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|addrbuf
decl_stmt|;
operator|*
name|buf
operator|++
operator|=
literal|'['
expr_stmt|;
operator|*
name|buf
operator|=
literal|'\0'
expr_stmt|;
comment|/* stpcpy() is cool */
name|inet_ntop
argument_list|(
name|AF_INET6
argument_list|,
operator|&
name|sin6_addr
operator|->
name|sin6_addr
argument_list|,
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|addrbuf
argument_list|)
operator|-
literal|1
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|buf
argument_list|,
literal|"]"
argument_list|)
expr_stmt|;
name|port
operator|=
name|sin6_addr
operator|->
name|sin6_port
expr_stmt|;
endif|#
directive|endif
block|}
name|loginfo
argument_list|(
literal|"Connection from %s:%d"
argument_list|,
name|addrbuf
argument_list|,
name|port
argument_list|)
expr_stmt|;
name|exit
argument_list|(
name|execute
argument_list|()
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|child_handler
specifier|static
name|void
name|child_handler
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|status
decl_stmt|;
name|pid_t
name|pid
init|=
name|waitpid
argument_list|(
operator|-
literal|1
argument_list|,
operator|&
name|status
argument_list|,
name|WNOHANG
argument_list|)
decl_stmt|;
if|if
condition|(
name|pid
operator|>
literal|0
condition|)
block|{
name|unsigned
name|reaped
init|=
name|children_reaped
decl_stmt|;
name|dead_child
index|[
name|reaped
operator|%
name|MAX_CHILDREN
index|]
operator|=
name|pid
expr_stmt|;
name|children_reaped
operator|=
name|reaped
operator|+
literal|1
expr_stmt|;
comment|/* XXX: Custom logging, since we don't wanna getpid() */
if|if
condition|(
name|verbose
condition|)
block|{
name|char
modifier|*
name|dead
init|=
literal|""
decl_stmt|;
if|if
condition|(
operator|!
name|WIFEXITED
argument_list|(
name|status
argument_list|)
operator|||
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
operator|>
literal|0
condition|)
name|dead
operator|=
literal|" (with error)"
expr_stmt|;
if|if
condition|(
name|log_syslog
condition|)
name|syslog
argument_list|(
name|LOG_INFO
argument_list|,
literal|"[%d] Disconnected%s"
argument_list|,
name|pid
argument_list|,
name|dead
argument_list|)
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"[%d] Disconnected%s\n"
argument_list|,
name|pid
argument_list|,
name|dead
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
break|break;
block|}
block|}
end_function
begin_ifndef
ifndef|#
directive|ifndef
name|NO_IPV6
end_ifndef
begin_function
DECL|function|socksetup
specifier|static
name|int
name|socksetup
parameter_list|(
name|int
name|port
parameter_list|,
name|int
modifier|*
modifier|*
name|socklist_p
parameter_list|)
block|{
name|int
name|socknum
init|=
literal|0
decl_stmt|,
modifier|*
name|socklist
init|=
name|NULL
decl_stmt|;
name|int
name|maxfd
init|=
operator|-
literal|1
decl_stmt|;
name|char
name|pbuf
index|[
name|NI_MAXSERV
index|]
decl_stmt|;
name|struct
name|addrinfo
name|hints
decl_stmt|,
modifier|*
name|ai0
decl_stmt|,
modifier|*
name|ai
decl_stmt|;
name|int
name|gai
decl_stmt|;
name|sprintf
argument_list|(
name|pbuf
argument_list|,
literal|"%d"
argument_list|,
name|port
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|hints
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|hints
argument_list|)
argument_list|)
expr_stmt|;
name|hints
operator|.
name|ai_family
operator|=
name|AF_UNSPEC
expr_stmt|;
name|hints
operator|.
name|ai_socktype
operator|=
name|SOCK_STREAM
expr_stmt|;
name|hints
operator|.
name|ai_protocol
operator|=
name|IPPROTO_TCP
expr_stmt|;
name|hints
operator|.
name|ai_flags
operator|=
name|AI_PASSIVE
expr_stmt|;
name|gai
operator|=
name|getaddrinfo
argument_list|(
name|NULL
argument_list|,
name|pbuf
argument_list|,
operator|&
name|hints
argument_list|,
operator|&
name|ai0
argument_list|)
expr_stmt|;
if|if
condition|(
name|gai
condition|)
name|die
argument_list|(
literal|"getaddrinfo() failed: %s\n"
argument_list|,
name|gai_strerror
argument_list|(
name|gai
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|ai
operator|=
name|ai0
init|;
name|ai
condition|;
name|ai
operator|=
name|ai
operator|->
name|ai_next
control|)
block|{
name|int
name|sockfd
decl_stmt|;
name|int
modifier|*
name|newlist
decl_stmt|;
name|sockfd
operator|=
name|socket
argument_list|(
name|ai
operator|->
name|ai_family
argument_list|,
name|ai
operator|->
name|ai_socktype
argument_list|,
name|ai
operator|->
name|ai_protocol
argument_list|)
expr_stmt|;
if|if
condition|(
name|sockfd
operator|<
literal|0
condition|)
continue|continue;
if|if
condition|(
name|sockfd
operator|>=
name|FD_SETSIZE
condition|)
block|{
name|error
argument_list|(
literal|"too large socket descriptor."
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|sockfd
argument_list|)
expr_stmt|;
continue|continue;
block|}
ifdef|#
directive|ifdef
name|IPV6_V6ONLY
if|if
condition|(
name|ai
operator|->
name|ai_family
operator|==
name|AF_INET6
condition|)
block|{
name|int
name|on
init|=
literal|1
decl_stmt|;
name|setsockopt
argument_list|(
name|sockfd
argument_list|,
name|IPPROTO_IPV6
argument_list|,
name|IPV6_V6ONLY
argument_list|,
operator|&
name|on
argument_list|,
sizeof|sizeof
argument_list|(
name|on
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Note: error is not fatal */
block|}
endif|#
directive|endif
if|if
condition|(
name|bind
argument_list|(
name|sockfd
argument_list|,
name|ai
operator|->
name|ai_addr
argument_list|,
name|ai
operator|->
name|ai_addrlen
argument_list|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|sockfd
argument_list|)
expr_stmt|;
continue|continue;
comment|/* not fatal */
block|}
if|if
condition|(
name|listen
argument_list|(
name|sockfd
argument_list|,
literal|5
argument_list|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|sockfd
argument_list|)
expr_stmt|;
continue|continue;
comment|/* not fatal */
block|}
name|newlist
operator|=
name|realloc
argument_list|(
name|socklist
argument_list|,
sizeof|sizeof
argument_list|(
name|int
argument_list|)
operator|*
operator|(
name|socknum
operator|+
literal|1
operator|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|newlist
condition|)
name|die
argument_list|(
literal|"memory allocation failed: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|socklist
operator|=
name|newlist
expr_stmt|;
name|socklist
index|[
name|socknum
operator|++
index|]
operator|=
name|sockfd
expr_stmt|;
if|if
condition|(
name|maxfd
operator|<
name|sockfd
condition|)
name|maxfd
operator|=
name|sockfd
expr_stmt|;
block|}
name|freeaddrinfo
argument_list|(
name|ai0
argument_list|)
expr_stmt|;
operator|*
name|socklist_p
operator|=
name|socklist
expr_stmt|;
return|return
name|socknum
return|;
block|}
end_function
begin_else
else|#
directive|else
end_else
begin_comment
comment|/* NO_IPV6 */
end_comment
begin_function
DECL|function|socksetup
specifier|static
name|int
name|socksetup
parameter_list|(
name|int
name|port
parameter_list|,
name|int
modifier|*
modifier|*
name|socklist_p
parameter_list|)
block|{
name|struct
name|sockaddr_in
name|sin
decl_stmt|;
name|int
name|sockfd
decl_stmt|;
name|sockfd
operator|=
name|socket
argument_list|(
name|AF_INET
argument_list|,
name|SOCK_STREAM
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|sockfd
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|memset
argument_list|(
operator|&
name|sin
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
name|sin
argument_list|)
expr_stmt|;
name|sin
operator|.
name|sin_family
operator|=
name|AF_INET
expr_stmt|;
name|sin
operator|.
name|sin_addr
operator|.
name|s_addr
operator|=
name|htonl
argument_list|(
name|INADDR_ANY
argument_list|)
expr_stmt|;
name|sin
operator|.
name|sin_port
operator|=
name|htons
argument_list|(
name|port
argument_list|)
expr_stmt|;
if|if
condition|(
name|bind
argument_list|(
name|sockfd
argument_list|,
operator|(
expr|struct
name|sockaddr
operator|*
operator|)
operator|&
name|sin
argument_list|,
sizeof|sizeof
name|sin
argument_list|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|sockfd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
operator|*
name|socklist_p
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|int
argument_list|)
argument_list|)
expr_stmt|;
operator|*
operator|*
name|socklist_p
operator|=
name|sockfd
expr_stmt|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|service_loop
specifier|static
name|int
name|service_loop
parameter_list|(
name|int
name|socknum
parameter_list|,
name|int
modifier|*
name|socklist
parameter_list|)
block|{
name|struct
name|pollfd
modifier|*
name|pfd
decl_stmt|;
name|int
name|i
decl_stmt|;
name|pfd
operator|=
name|xcalloc
argument_list|(
name|socknum
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|pollfd
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
name|socknum
condition|;
name|i
operator|++
control|)
block|{
name|pfd
index|[
name|i
index|]
operator|.
name|fd
operator|=
name|socklist
index|[
name|i
index|]
expr_stmt|;
name|pfd
index|[
name|i
index|]
operator|.
name|events
operator|=
name|POLLIN
expr_stmt|;
block|}
name|signal
argument_list|(
name|SIGCHLD
argument_list|,
name|child_handler
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|poll
argument_list|(
name|pfd
argument_list|,
name|socknum
argument_list|,
operator|-
literal|1
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|EINTR
condition|)
block|{
name|error
argument_list|(
literal|"poll failed, resuming: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|sleep
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|socknum
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|revents
operator|&
name|POLLIN
condition|)
block|{
name|struct
name|sockaddr_storage
name|ss
decl_stmt|;
name|unsigned
name|int
name|sslen
init|=
sizeof|sizeof
argument_list|(
name|ss
argument_list|)
decl_stmt|;
name|int
name|incoming
init|=
name|accept
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|,
operator|(
expr|struct
name|sockaddr
operator|*
operator|)
operator|&
name|ss
argument_list|,
operator|&
name|sslen
argument_list|)
decl_stmt|;
if|if
condition|(
name|incoming
operator|<
literal|0
condition|)
block|{
switch|switch
condition|(
name|errno
condition|)
block|{
case|case
name|EAGAIN
case|:
case|case
name|EINTR
case|:
case|case
name|ECONNABORTED
case|:
continue|continue;
default|default:
name|die
argument_list|(
literal|"accept returned %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
name|handle
argument_list|(
name|incoming
argument_list|,
operator|(
expr|struct
name|sockaddr
operator|*
operator|)
operator|&
name|ss
argument_list|,
name|sslen
argument_list|)
expr_stmt|;
block|}
block|}
block|}
block|}
end_function
begin_function
DECL|function|serve
specifier|static
name|int
name|serve
parameter_list|(
name|int
name|port
parameter_list|)
block|{
name|int
name|socknum
decl_stmt|,
modifier|*
name|socklist
decl_stmt|;
name|socknum
operator|=
name|socksetup
argument_list|(
name|port
argument_list|,
operator|&
name|socklist
argument_list|)
expr_stmt|;
if|if
condition|(
name|socknum
operator|==
literal|0
condition|)
name|die
argument_list|(
literal|"unable to allocate any listen sockets on port %u"
argument_list|,
name|port
argument_list|)
expr_stmt|;
return|return
name|service_loop
argument_list|(
name|socknum
argument_list|,
name|socklist
argument_list|)
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
name|int
name|port
init|=
name|DEFAULT_GIT_PORT
decl_stmt|;
name|int
name|inetd_mode
init|=
literal|0
decl_stmt|;
name|int
name|i
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
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--port="
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|char
modifier|*
name|end
decl_stmt|;
name|unsigned
name|long
name|n
decl_stmt|;
name|n
operator|=
name|strtoul
argument_list|(
name|arg
operator|+
literal|7
argument_list|,
operator|&
name|end
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|arg
index|[
literal|7
index|]
operator|&&
operator|!
operator|*
name|end
condition|)
block|{
name|port
operator|=
name|n
expr_stmt|;
continue|continue;
block|}
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--inetd"
argument_list|)
condition|)
block|{
name|inetd_mode
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
condition|)
block|{
name|verbose
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
literal|"--syslog"
argument_list|)
condition|)
block|{
name|log_syslog
operator|=
literal|1
expr_stmt|;
name|openlog
argument_list|(
literal|"git-daemon"
argument_list|,
literal|0
argument_list|,
name|LOG_DAEMON
argument_list|)
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
literal|"--export-all"
argument_list|)
condition|)
block|{
name|export_all_trees
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--timeout="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|timeout
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|10
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--init-timeout="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|init_timeout
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|15
argument_list|)
expr_stmt|;
block|}
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
name|ok_paths
operator|=
operator|&
name|argv
index|[
name|i
operator|+
literal|1
index|]
expr_stmt|;
break|break;
block|}
elseif|else
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
block|{
name|ok_paths
operator|=
operator|&
name|argv
index|[
name|i
index|]
expr_stmt|;
break|break;
block|}
name|usage
argument_list|(
name|daemon_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|inetd_mode
condition|)
block|{
name|fclose
argument_list|(
name|stderr
argument_list|)
expr_stmt|;
comment|//FIXME: workaround
return|return
name|execute
argument_list|()
return|;
block|}
else|else
block|{
return|return
name|serve
argument_list|(
name|port
argument_list|)
return|;
block|}
block|}
end_function
end_unit
