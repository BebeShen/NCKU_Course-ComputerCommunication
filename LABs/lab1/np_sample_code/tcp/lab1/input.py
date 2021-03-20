
with open('data_200kb.txt','w') as f:
    for i in range(0,200*100):
        f.write(f"test {i}\n")
