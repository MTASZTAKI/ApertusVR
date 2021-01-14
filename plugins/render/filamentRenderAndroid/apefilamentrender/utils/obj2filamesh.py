import os
from itertools import compress

def main():
    print(os.getcwd())
    filesAndDirs = os.listdir()
    
    objFiles = list(filter(lambda x: x.split('.')[-1] == 'obj',filesAndDirs))
    objFiles = list(map(lambda x: x.split('.')[0],objFiles))
    
    filameshFiles = list(filter(lambda x: x.split('.')[-1] == 'filamesh',filesAndDirs))
    filameshFiles = list(map(lambda x: x.split('.')[0],filameshFiles))
    
    for objFileName in objFiles:
        if objFileName in filameshFiles:
            continue
        
        command = 'filamesh ' + objFileName + '.obj ' + objFileName + '.filamesh'
        os.system(command)
    
    dirs = list(filter(lambda x: len(x.split('.')) == 1, filesAndDirs))
    
    for dir in dirs:
        os.chdir(dir)
        main()
        os.chdir('..')
    
    
if __name__ == "__main__":
    main()
