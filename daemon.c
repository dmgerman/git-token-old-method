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
file|<netdb.h>
end_include
begin_include
include|#
directive|include
file|<netinet/in.h>
end_include
begin_decl_stmt
DECL|variable|daemon_usage
specifier|static
specifier|const
name|char
name|daemon_usage
index|[]
init|=
literal|"git-daemon [--inetd | --port=n]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|upload
specifier|static
name|int
name|upload
parameter_list|(
name|char
modifier|*
name|dir
parameter_list|,
name|int
name|dirlen
parameter_list|)
block|{
if|if
condition|(
name|chdir
argument_list|(
name|dir
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|chdir
argument_list|(
literal|".git"
argument_list|)
expr_stmt|;
comment|/* 	 * Security on the cheap. 	 * 	 * We want a readable HEAD, usable "objects" directory, and  	 * a "git-daemon-export-ok" flag that says that the other side 	 * is ok with us doing this. 	 */
if|if
condition|(
name|access
argument_list|(
literal|"git-daemon-export-ok"
argument_list|,
name|F_OK
argument_list|)
operator|||
name|access
argument_list|(
literal|"objects/00"
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
return|return
operator|-
literal|1
return|;
comment|/* 	 * We'll ignore SIGTERM from now on, we have a 	 * good client. 	 */
name|signal
argument_list|(
name|SIGTERM
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
comment|/* git-upload-pack only ever reads stuff, so this is safe */
name|execlp
argument_list|(
literal|"git-upload-pack"
argument_list|,
literal|"git-upload-pack"
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
argument_list|,
name|len
operator|-
literal|16
argument_list|)
return|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"got bad connection '%s'\n"
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
name|socklen_t
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
name|socklen_t
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
name|socklen_t
name|addrlen
parameter_list|)
block|{
name|pid_t
name|pid
init|=
name|fork
argument_list|()
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
name|pid_t
name|pid
init|=
name|waitpid
argument_list|(
operator|-
literal|1
argument_list|,
name|NULL
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
continue|continue;
block|}
break|break;
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
name|fd_set
name|fds_init
decl_stmt|,
name|fds
decl_stmt|;
name|char
name|pbuf
index|[
name|NI_MAXSERV
index|]
decl_stmt|;
name|signal
argument_list|(
name|SIGCHLD
argument_list|,
name|child_handler
argument_list|)
expr_stmt|;
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
name|FD_ZERO
argument_list|(
operator|&
name|fds_init
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
name|FD_SET
argument_list|(
name|sockfd
argument_list|,
operator|&
name|fds_init
argument_list|)
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
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|i
decl_stmt|;
name|fds
operator|=
name|fds_init
expr_stmt|;
if|if
condition|(
name|select
argument_list|(
name|maxfd
operator|+
literal|1
argument_list|,
operator|&
name|fds
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|NULL
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
literal|"select failed, resuming: %s"
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
name|int
name|sockfd
init|=
name|socklist
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|FD_ISSET
argument_list|(
name|sockfd
argument_list|,
operator|&
name|fds
argument_list|)
condition|)
block|{
name|struct
name|sockaddr_storage
name|ss
decl_stmt|;
name|socklen_t
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
name|sockfd
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
return|return
name|execute
argument_list|()
return|;
return|return
name|serve
argument_list|(
name|port
argument_list|)
return|;
block|}
end_function
end_unit
