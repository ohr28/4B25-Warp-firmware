# import the required libraries 
import random 
import matplotlib.pyplot as plt 
    
# store the random numbers in a  
# list 
nums = [] 
mu = 4033.6
sigma = 9.183
count = 10004
    
for i in range(count): 
    temp = random.gauss(mu, sigma)
    nums.append(temp) 
        
# plotting a graph 
plt.hist(nums, 100) 
plt.show()

for i in nums:
    print(i)

sum = 0
for i in range(count):
    sum += nums[i]
avg = sum / count

for i in range(count):
    nums[i] -= avg

int_nums = [0]

for i in range(count):
    int_nums.append(((nums[i] + nums[i-1]) / 2) + int_nums[i-1])



#plt.hist(int_nums, 100) 
#plt.show()


int_int_nums = [0, 0]

for i in range(count):
    int_int_nums.append(((int_nums[i] + int_nums[i-1]) / 2) + int_int_nums[i-1])


#plt.hist(int_nums, 1000) 
#plt.show()