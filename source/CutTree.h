pd_table['allsum'] = pd_table.dist_ans1_ans2 + pd_table.dist11ans1 +\
    pd_table['dist12ans1'] + pd_table.dist21ans2 + pd_table.dist22ans2
pd_table['dist11_21'] = pd_table.dist_ans1_ans2 + pd_table.dist11ans1 +\
    pd_table.dist12ans1 + pd_table.dist21ans2 + pd_table.dist22ans2
pd_table['dist11_22'] = pd_table.dist_ans1_ans2 + pd_table.dist11ans1 +\
    pd_table.dist22ans2

pd_table['dist12_21'] = pd_table.dist_ans1_ans2 + pd_table.dist12ans1 + pd_table.dist21ans2
pd_table['dist12_22'] = pd_table.dist_ans1_ans2 + pd_table.dist12ans1 + pd_table.dist22ans2
pd_table['dist11_12'] = pd_table.dist11ans1 + pd_table.dist12ans1
pd_table['dist21_22'] = pd_table.dist21ans2 + pd_table.dist22ans2
pd_table['max_cross_dist'] = pd_table.apply(lambda x : max(x.dist11_21, x.dist11_22, x.dist12_21, x.dist12_22), axis = 1)
pd_table['max_common_dist'] = pd_table.apply(lambda x : max(x.dist11_12, x.dist21_22), axis = 1)

pd_table['min_cross_dist'] = pd_table.apply(lambda x : min(x.dist11_21, x.dist11_22, x.dist12_21, x.dist12_22), axis = 1)
pd_table['min_common_dist'] = pd_table.apply(lambda x : min(x.dist11_12, x.dist21_22), axis = 1)
pd_table['mean_common_dist'] = (pd_table['dist11_12'] + pd_table['dist21_22']) / 2
pd_table['diff_dist'] = pd_table['min_cross_dist'] - pd_table['max_common_dist']
pd_table['relative_dist'] = pd_table['min_cross_dist'] / pd_table['max_common_dist']
pd_table['proportion_dist'] = pd_table['min_cross_dist'] / (pd_table['max_common_dist']\
                                                                                  + pd_table['min_cross_dist'])
pd_table['diff_sum_proportion'] = (pd_table['min_cross_dist'] - pd_table['max_common_dist'])\
    / (pd_table['max_common_dist'] + pd_table['min_cross_dist'])