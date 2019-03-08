import os
import sys

# p1_1 selection sort test
for fname in ['1_1test1.txt', '1_1test2.txt', '1_1test3.txt']:
    os.rename(fname, 'input.txt')
    os.system('./p1_1')

    with open('output.txt') as f:
        data = f.read()
    
    nums = [int(x) for x in data.split(' ') if x.isdigit()]
    
    prev = nums[0]
    for num in nums[1:]:
        if prev > num:
            print('error occured from p1_1 in file {}'.format(fname))
            sys.exit(1)
        else:
            prev = num
    
    os.rename('input.txt', fname)

print('p1_1 success')

# p1_2 test
os.system('./p1_2 student_information.txt output.txt')
with open('student_information.txt') as f:
    data = f.read()
    predicted = '\n'.join(data.split('\n')[1:])

with open('output.txt') as f:
    output = f.read()

if predicted != output:
    print('error occured from p1_2')
    sys.exit(1)

print('p1_2 success')
sys.exit(0)
