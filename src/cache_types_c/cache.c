
#include <stdio.h>
#define True 1
#define False 0
#define len 39

//-------------Global Variables----------------//
int cacheSize;
int bytesPerBlock;
int cacheType = 1; //0 for direct mapped, 1 for fully associative 
int linesInCache;
int n; //For n-way set associative
int numSets;
int setSize;
const int ADDRESSES[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 
				256, 272, 288, 304, 320, 0, 16, 32, 48, 64, 284, 12, 164, 324, 156, 152, 
				284, 60, 156, 44, 204, 228, 164};

struct addrInfo{ //Store info about the addresses given so it doesn't have to be recalculated
	int addrGiven;
	int tag;
	int indexInCache;
	int offset;
	int set; //Only for n-way set associative cache
};

struct cacheEntry{ //One line in the cache
	int tag;
	int valid;
	int set; //Only for n-way set associative 
};

struct cacheEntry cache[512]; //The cache itself 
struct addrInfo addrs[len]; //The info regarding the addresses stored in ADDRESSES[] 

int fifoCounter; //The index of the element to be removed next, loops back to 0 if 
		 //it were to be equal to the num of lines in the cache

void directMapped() {
	int i;
	for (i = 0; i < len; i++) {
		struct addrInfo currentAddress = addrs[i];
		if (currentAddress.tag == cache[currentAddress.indexInCache].tag && 
					cache[currentAddress.indexInCache].valid == 1) {
		//The tags match and valid is true, so it is a match
		printf("  %d:  HIT  (Tag/Index/Offset: %d/%d/%d)\n",ADDRESSES[i],currentAddress.tag, 
					currentAddress.indexInCache, currentAddress.offset);
		} else {
		//Otherwise its not a match, update cache accordingly
		printf("  %d:  MISS (Tag/Index/Offset: %d/%d/%d)\n",ADDRESSES[i],currentAddress.tag, 
					currentAddress.indexInCache, currentAddress.offset);
		cache[currentAddress.indexInCache].valid = 1;
		cache[currentAddress.indexInCache].tag = addrs[i].tag;
		} 
	}
	printf("Cache contents (for each cache row index)\n");
	int j;
	for (j = 0; j < linesInCache; j++) {
		if (cache[j].valid == 0) {
			printf("  %d: VALID: False; Tag: - (set #: %d)\n", j, j);
		} else {
			printf("  %d: VALID: True ; Tag: %d (set #: %d)\n", j, cache[j].tag, j);
		}
	}		
} 
void associative() {
	fifoCounter = 0;
	int i;
	for (i = 0; i < len; i++) { //Loop through ADDRESSES
		int match = tagValidMatch(addrs[i].tag);
		if (match != -1) {
			//The tag is in the cache
			printf("  %d:  HIT  (Tag/Set #/Offset: %d/0/%d)\n",ADDRESSES[i],addrs[i].tag, 
					addrs[i].offset);

			
		} else {
			//The tag is not in the cache, need to put the tag in the cache at pos fifoList.Counter
			printf("  %d:  MISS (Tag/Set #/Offset: %d/0/%d)\n",ADDRESSES[i],addrs[i].tag, 
					addrs[i].offset);
			cache[fifoCounter].tag = addrs[i].tag;
			cache[fifoCounter].valid = 1;
			if (fifoCounter == (linesInCache - 1))
				fifoCounter = 0;
			else 
				fifoCounter++;
			 
		}				
	}
	printf("Cache contents (for each cache row index)\n");
	int j;
	for (j = 0; j < linesInCache; j++) {
		if (cache[j].valid == 0) 
			printf(" %d: VALID: False; Tag: - (Set #: 0)\n", j, addrs[j].indexInCache);
		else 
			printf(" %d: VALID: True ; Tag: %d (Set #: 0)\n", j, cache[j].tag);
			
	}			
}

int tagValidMatch(int tagGiven) { //Returns 1 if the given tag is in the cache and the valid bit is true, 0 otherwise
	int i;
	for (i = 0; i < linesInCache; i++) {
		if ((cache[i].tag == tagGiven) && (cache[i].valid == 1)) 
			return i;
	}
	return -1;	
}

void nway() {
	int fifo[numSets];
	int copy[numSets];
	
	int i;
	for (i = 0; i < numSets; i++) {
		fifo[i] = i * setSize;
		copy[i] = fifo[i];
	}
	
	int match = 0;
	int remove;
	for (i = 0; i < len; i++) {
		remove = fifo[addrs[i].set];
		match = 0;
		int j;
		for (j = 0; j < linesInCache; j++) {
			if ((cache[j].valid == 1) && (cache[j].tag == addrs[i].tag) && (cache[j].set == addrs[i].set)) {
				//Got a hit, the valid bit is set and the tag and sets match
				match = 1;
				break;					
			}
		}
		if (match == 1) {
			printf("  %d:  HIT  (Tag/Set #/Offset: %d/%d/%d)\n",ADDRESSES[i], addrs[i].tag,addrs[i].set, addrs[i].offset);	
		} else {
			printf("  %d:  MISS  (Tag/Set #/Offset: %d/%d/%d)\n",ADDRESSES[i], addrs[i].tag,addrs[i].set, addrs[i].offset);
		}
		
		if (match == 0) {
			//Check if the set is full
			int fullSet = 1;
			int x;
			for (x = 0; x < linesInCache; x++) {
				if ((cache[x].valid == 0) && (cache[x].set == addrs[i].set)) {
					fullSet = 0;
				}
			}
			if (fullSet == 1) {
				cache[remove].tag = addrs[i].tag;
				cache[remove].valid = 1;
				if (remove == setSize + copy[addrs[i].set] - 1) 
					fifo[addrs[i].set] = copy[addrs[i].set]; //Loop back
				else 
					 fifo[addrs[i].set]++; //Can just incriment
			} else {
				// Can just place in the cache at the first open valid slot
				for (j = 0; i < linesInCache; j++) {
					if ((cache[j].valid == 0) && (cache[j].set == addrs[i].set)) {
						cache[j].tag = addrs[i].tag;
						cache[j].valid = 1;
						break;
					} 
									
				}
			}
			
			
		}
	} //end for loop	
	printf("Cache contents (for each cache row index)\n");
	int j;
        for (j = 0; j < linesInCache; j++) {
                if (cache[j].valid == 0)
                        printf(" %d: VALID: False; Tag: - (Set #: %d)\n", j, cache[j].set);
                else
                        printf(" %d: VALID: True ; Tag: %d (Set #: %d)\n", j, cache[j].tag, cache[j].set);

        }	
}
int main(int argc, char* argv[]) {	

	printf("Cache total size (in bytes)?: ");
	scanf("%d", &cacheSize);

	printf("Bytes per block?: ");
	scanf("%d", &bytesPerBlock);
	
	char inputStr[8];
	
	printf("Direct-mapped cache? [y/N]");
	scanf("%s", inputStr);
	
	if (strcmp(inputStr, "y") == 0) cacheType = 0;
 	
	if (cacheType == 1) {
		char in[8];
		printf("Fully Associative Cache? [y/N]");
		scanf("%s", in);
		if (strcmp(in, "y") != 0) {
			printf("Then it must be n-way set associative; What is n?: ");
			scanf("%d", &n);
			cacheType = 2;
		}
	}
	linesInCache = cacheSize / bytesPerBlock;
	
		
	if (cacheType == 0) { //Direct Mapped 
		//Fill the addrInfo structs in the addrs array to the appropiate values
		int i;
		for (i = 0; i < len; i++) {
			addrs[i].addrGiven = ADDRESSES[i];
			addrs[i].tag = ADDRESSES[i] / bytesPerBlock / linesInCache;
			addrs[i].indexInCache = (ADDRESSES[i] / bytesPerBlock) % linesInCache;
			addrs[i].offset = ADDRESSES[i] % bytesPerBlock;
			cache[i].valid = 0;
		}
		//Everything is set up, we can call the direct-mapped method
		directMapped();
	} 
	if (cacheType == 1) { //Fully associative
		int i;
		for (i = 0; i < len; i++) {
			addrs[i].addrGiven = ADDRESSES[i];
			addrs[i].tag = ADDRESSES[i] / bytesPerBlock;
			addrs[i].offset = ADDRESSES[i] % bytesPerBlock;
			cache[i].valid = 0;
		}
		associative();		
	}
	if (cacheType == 2) { //n-way set associative 
		printf("n-way cache");
		numSets = linesInCache / n;
		setSize = linesInCache / numSets;
		
		int l = bytesPerBlock;
		int i;
		for (i = 0; i < len; i++) {
			addrs[i].addrGiven = ADDRESSES[i];
			addrs[i].tag = ADDRESSES[i] / l / numSets;
			addrs[i].offset = ADDRESSES[i] % l;
			addrs[i].set = (ADDRESSES[i] / l) % numSets;
			cache[i].valid = 0;
			cache[i].set = i / setSize;
		}
		nway();
		
	}
	return 0;		
}

