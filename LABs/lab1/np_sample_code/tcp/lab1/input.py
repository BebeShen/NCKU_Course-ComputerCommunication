
with open('data.txt','w') as f:
    for i in range(0,200*1000*100):
        f.write(f"test {i}\n")