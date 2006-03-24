begin_unit
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
begin_define
DECL|macro|PARENT1
define|#
directive|define
name|PARENT1
value|1
end_define
begin_define
DECL|macro|PARENT2
define|#
directive|define
name|PARENT2
value|2
end_define
begin_define
DECL|macro|UNINTERESTING
define|#
directive|define
name|UNINTERESTING
value|4
end_define
begin_function
DECL|function|interesting
specifier|static
name|struct
name|commit
modifier|*
name|interesting
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
return|return
name|commit
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_comment
comment|/*  * A pathological example of how this thing works.  *  * Suppose we had this commit graph, where chronologically  * the timestamp on the commit are A<= B<= C<= D<= E<= F  * and we are trying to figure out the merge base for E and F  * commits.  *  *                  F  *                 / \  *            E   A   D  *             \ /   /    *              B   /  *               \ /  *                C  *  * First we push E and F to list to be processed.  E gets bit 1  * and F gets bit 2.  The list becomes:  *  *     list=F(2) E(1), result=empty  *  * Then we pop F, the newest commit, from the list.  Its flag is 2.  * We scan its parents, mark them reachable from the side that F is  * reachable from, and push them to the list:  *  *     list=E(1) D(2) A(2), result=empty  *  * Next pop E and do the same.  *  *     list=D(2) B(1) A(2), result=empty  *  * Next pop D and do the same.  *  *     list=C(2) B(1) A(2), result=empty  *  * Next pop C and do the same.  *  *     list=B(1) A(2), result=empty  *  * Now it is B's turn.  We mark its parent, C, reachable from B's side,  * and push it to the list:  *  *     list=C(3) A(2), result=empty  *  * Now pop C and notice it has flags==3.  It is placed on the result list,  * and the list now contains:  *  *     list=A(2), result=C(3)  *  * We pop A and do the same.  *   *     list=B(3), result=C(3)  *  * Next, we pop B and something very interesting happens.  It has flags==3  * so it is also placed on the result list, and its parents are marked  * uninteresting, retroactively, and placed back on the list:  *  *    list=C(7), result=C(7) B(3)  *   * Now, list does not have any interesting commit.  So we find the newest  * commit from the result list that is not marked uninteresting.  Which is  * commit B.  *  *  * Another pathological example how this thing can fail to mark an ancestor  * of a merge base as UNINTERESTING without the postprocessing phase.  *  *		  2  *		  H  *	    1    / \  *	    G   A   \  *	    |\ /     \   *	    | B       \  *	    |  \       \  *	     \  C       F  *	      \  \     /   *	       \  D   /     *		\ |  /  *		 \| /  *		  E  *  *	 list			A B C D E F G H  *	 G1 H2			- - - - - - 1 2  *	 H2 E1 B1		- 1 - - 1 - 1 2  *	 F2 E1 B1 A2		2 1 - - 1 2 1 2  *	 E3 B1 A2		2 1 - - 3 2 1 2  *	 B1 A2			2 1 - - 3 2 1 2  *	 C1 A2			2 1 1 - 3 2 1 2  *	 D1 A2			2 1 1 1 3 2 1 2  *	 A2			2 1 1 1 3 2 1 2  *	 B3			2 3 1 1 3 2 1 2  *	 C7			2 3 7 1 3 2 1 2  *  * At this point, unfortunately, everybody in the list is  * uninteresting, so we fail to complete the following two  * steps to fully marking uninteresting commits.  *  *	 D7			2 3 7 7 3 2 1 2  *	 E7			2 3 7 7 7 2 1 2  *  * and we end up showing E as an interesting merge base.  */
end_comment
begin_decl_stmt
DECL|variable|show_all
specifier|static
name|int
name|show_all
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|mark_reachable_commits
specifier|static
name|void
name|mark_reachable_commits
parameter_list|(
name|struct
name|commit_list
modifier|*
name|result
parameter_list|,
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|tmp
decl_stmt|;
comment|/* 	 * Postprocess to fully contaminate the well. 	 */
for|for
control|(
name|tmp
operator|=
name|result
init|;
name|tmp
condition|;
name|tmp
operator|=
name|tmp
operator|->
name|next
control|)
block|{
name|struct
name|commit
modifier|*
name|c
init|=
name|tmp
operator|->
name|item
decl_stmt|;
comment|/* Reinject uninteresting ones to list, 		 * so we can scan their parents. 		 */
if|if
condition|(
name|c
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
name|commit_list_insert
argument_list|(
name|c
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
block|}
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|c
init|=
name|list
operator|->
name|item
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|tmp
operator|=
name|list
expr_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
comment|/* Anything taken out of the list is uninteresting, so 		 * mark all its parents uninteresting.  We do not 		 * parse new ones (we already parsed all the relevant 		 * ones). 		 */
name|parents
operator|=
name|c
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|p
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
operator|)
condition|)
block|{
name|p
operator|->
name|object
operator|.
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|commit_list_insert
argument_list|(
name|p
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
block|}
block|}
block|}
block|}
end_function
begin_function
DECL|function|merge_base
specifier|static
name|int
name|merge_base
parameter_list|(
name|struct
name|commit
modifier|*
name|rev1
parameter_list|,
name|struct
name|commit
modifier|*
name|rev2
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|list
init|=
name|NULL
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|result
init|=
name|NULL
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|tmp
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|rev1
operator|==
name|rev2
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|rev1
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|parse_commit
argument_list|(
name|rev1
argument_list|)
expr_stmt|;
name|parse_commit
argument_list|(
name|rev2
argument_list|)
expr_stmt|;
name|rev1
operator|->
name|object
operator|.
name|flags
operator||=
literal|1
expr_stmt|;
name|rev2
operator|->
name|object
operator|.
name|flags
operator||=
literal|2
expr_stmt|;
name|insert_by_date
argument_list|(
name|rev1
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
name|insert_by_date
argument_list|(
name|rev2
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
while|while
condition|(
name|interesting
argument_list|(
name|list
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|int
name|flags
init|=
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
literal|7
decl_stmt|;
name|tmp
operator|=
name|list
expr_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
if|if
condition|(
name|flags
operator|==
literal|3
condition|)
block|{
name|insert_by_date
argument_list|(
name|commit
argument_list|,
operator|&
name|result
argument_list|)
expr_stmt|;
comment|/* Mark parents of a found merge uninteresting */
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
block|}
name|parents
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
operator|(
name|p
operator|->
name|object
operator|.
name|flags
operator|&
name|flags
operator|)
operator|==
name|flags
condition|)
continue|continue;
name|parse_commit
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|p
operator|->
name|object
operator|.
name|flags
operator||=
name|flags
expr_stmt|;
name|insert_by_date
argument_list|(
name|p
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
operator|!
name|result
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|result
operator|->
name|next
operator|&&
name|list
condition|)
name|mark_reachable_commits
argument_list|(
name|result
argument_list|,
name|list
argument_list|)
expr_stmt|;
while|while
condition|(
name|result
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|result
operator|->
name|item
decl_stmt|;
name|result
operator|=
name|result
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_all
condition|)
return|return
literal|0
return|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|merge_base_usage
specifier|static
specifier|const
name|char
name|merge_base_usage
index|[]
init|=
literal|"git-merge-base [--all]<commit-id><commit-id>"
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
name|struct
name|commit
modifier|*
name|rev1
decl_stmt|,
modifier|*
name|rev2
decl_stmt|;
name|unsigned
name|char
name|rev1key
index|[
literal|20
index|]
decl_stmt|,
name|rev2key
index|[
literal|20
index|]
decl_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
operator|&&
name|argv
index|[
literal|1
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
name|char
modifier|*
name|arg
init|=
name|argv
index|[
literal|1
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-a"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--all"
argument_list|)
condition|)
name|show_all
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|merge_base_usage
argument_list|)
expr_stmt|;
name|argc
operator|--
expr_stmt|;
name|argv
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|!=
literal|3
operator|||
name|get_sha1
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|rev1key
argument_list|)
operator|||
name|get_sha1
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
name|rev2key
argument_list|)
condition|)
name|usage
argument_list|(
name|merge_base_usage
argument_list|)
expr_stmt|;
name|rev1
operator|=
name|lookup_commit_reference
argument_list|(
name|rev1key
argument_list|)
expr_stmt|;
name|rev2
operator|=
name|lookup_commit_reference
argument_list|(
name|rev2key
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rev1
operator|||
operator|!
name|rev2
condition|)
return|return
literal|1
return|;
return|return
name|merge_base
argument_list|(
name|rev1
argument_list|,
name|rev2
argument_list|)
return|;
block|}
end_function
end_unit
