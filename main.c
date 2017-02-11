// NOTES
//	- I am not the most proficient C console programmer in the world and have resorted to using things like Sleep, system(), and other
//		functions that are usually frowned upon.
//	- I have done very very limited Linux work in C/C++, and as such, have made the Linux version a bit less full-fleshed out. I
//		believe it needs to go without saying that I would put in the time to making all the features fully-cross compatible
//		if this were a program I was writing commercially, and if I even had a version of Linux to play around with it on*
//	- I avoided OOP for this task as it seemed overkill. This is a simple data task that at its base is 4 numbers.
//	- Const protection is lame

//	- *Thanks to Abner for running this on Linux to make sure it isn't a completely mess :)

#include <stdio.h>

#include <stdint.h>
typedef int32_t int32;
typedef int32 bool32;
typedef uint32_t uint32;

#define ArrayCount(x)	(sizeof(x) / sizeof((x)[0]))

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <Windows.h>

#define BLACK			0
#define CYAN			3
#define LIGHTGRAY		7
#define DEFAULT			LIGHTGRAY

void
ClearConsole()
{
	system("CLS");
}

void
SetColor(char FColor, char BColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (BColor << 4) | FColor);
}

void
SetTextCursorPos(SHORT X, SHORT Y)
{
	COORD Pos = { X, Y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void
PauseFor(DWORD Milliseconds)
{
	// Oh gross Bret's using that nasty Sleep function. What a terrible programmer!
	//Sleep(Milliseconds);
}

bool32
GetReturnKeyDown()
{
	return GetAsyncKeyState(VK_RETURN) & 1;
}

void
WaitForInput()
{
	system("PAUSE");
}

#else

#include <unistd.h>
#include <term.h>

void
ClearConsole()
{
	system("clear");
}

void
SetColor(char FColor, char BColor)
{
	// NOTE(bret): I've never done color stuff in Linux terminals and it looks like it's a bit more intensive than Windows. I'm going to pass on that for this project
}

void
SetTextCursorPos(int32 X, int32 Y)
{
	// NOTE(bret): Thanks http://www.cplusplus.com/forum/general/41709/#msg225145 (wow, his Windows implementation is the same as mine. How original. Duoas gets an F)
	int err;
	if (!cur_term)
		if (setupterm(NULL, STDOUT_FILENO, &err) == ERR)
			return;
	putp(tparm(tigetstr("cup"), Y, X, 0, 0, 0, 0, 0, 0, 0));
}

void
PauseFor(useconds_t Milliseconds)
{
	// I don't know if usleep has a stigma around it like Window's Sleep function, but we'll say it doesn't
	usleep(Milliseconds);
}

bool32
GetReturnKeyDown()
{
	// NOTE(bret): Also never done input handling on Linux, and it seems to also be super complex. Skimping on this
	return 0;
}

void
WaitForInput()
{
	system("read");
}

#endif

typedef struct bucket
{
	char Name[6];

	uint32 Capacity;
	uint32 Water;

	uint32 Remainder;
} bucket;

void
StrCpy(char *Dst, char *Src, int32 Length)
{
	while ((*Src) && Length)
	{
		*Dst++ = *Src++;
		--Length;
	}

	*Dst = '\0';
}

// Since the name string is first, we can convert the buckets straight to char *
// This way, if a bucket is sent as null, it'll just convert it to ((char *)0) which won't crash
void
Log(char *Str, bucket *Bucket1, bucket *Bucket2)
{
	char Buffer[256];
	_snprintf(Buffer, sizeof(Buffer), Str, (char *)Bucket1, (char *)Bucket2);
	printf(Buffer);
}

void
UpdateBucketDisplay(bucket *Bucket, SHORT YPos)
{
	// Write stats
	SetTextCursorPos(0, YPos);
	printf("\t[ %d / %d ]", Bucket->Water, Bucket->Capacity);

	// Draw bucket
	SetTextCursorPos(0, YPos + 1);
	printf("\t");
	SetColor(CYAN, CYAN);
	for (uint32 i = 0; i < Bucket->Capacity; ++i)
	{
		if (i >= Bucket->Water)
			SetColor(DEFAULT, DEFAULT);
		printf("  ");
	}
	SetColor(DEFAULT, BLACK);
}

void
MoveCursorPosToStep(uint32 Step)
{
	SetTextCursorPos(0, 8 + Step);
}

void
UpdateBucketsDisplay(bucket *LargeBucket, bucket *SmallBucket, int32 StepsTaken)
{
	UpdateBucketDisplay(LargeBucket, 2);
	UpdateBucketDisplay(SmallBucket, 5);
	
	if (GetReturnKeyDown())
		PauseFor(100);
	else
		PauseFor(1000);
}

void
PromptInt(char *Str, int *N)
{
	printf(Str);
	// NOTE(bret): Not the best way of checking input but w/e
	scanf("%d", N);
}

__inline bool32
BucketsHaveGoal(bucket *LargeBucket, bucket *SmallBucket, uint32 Goal)
{
	bool32 Result = (LargeBucket->Water == Goal) || (SmallBucket->Water == Goal);
	return Result;
}

__inline bool32
BucketIsFull(bucket *Bucket)
{
	bool32 Result = (Bucket->Water == Bucket->Capacity);
	return Result;
}

__inline bool32
BucketIsEmpty(bucket *Bucket)
{
	bool32 Result = (Bucket->Water == 0);
	return Result;
}

void
FillBucket(bucket *Bucket)
{
	Bucket->Water = Bucket->Capacity;
	Log("Filled %s\n", Bucket, 0);
}

// TODO(bret): See if there's a cleaner math way to do this
void
TransferBucket(bucket *From, bucket *To)
{
	uint32 AmountToTake = To->Capacity - To->Water;

	if (From->Water <= AmountToTake)
	{
		AmountToTake = From->Water;
	}

	To->Water += AmountToTake;
	From->Water -= AmountToTake;

	Log("Transferred %s to %s\n", From, To);
}

void
DumpBucket(bucket *Bucket)
{
	Bucket->Water = 0;
	Log("Dumped %s\n", Bucket, 0);
}

void
InitBuckets(bucket *LargeBucket, uint32 LargeCapacity, bucket *SmallBucket, uint32 SmallCapacity)
{
	if (LargeCapacity < SmallCapacity)
	{
		LargeCapacity ^= SmallCapacity;
		SmallCapacity ^= LargeCapacity;
		LargeCapacity ^= SmallCapacity;
	}

	LargeBucket->Capacity = LargeCapacity;
	SmallBucket->Capacity = SmallCapacity;
	LargeBucket->Water = SmallBucket->Water = 0;

	LargeBucket->Remainder = (SmallBucket->Capacity) ? (LargeBucket->Capacity % SmallBucket->Capacity) : 0;
	SmallBucket->Remainder = SmallBucket->Capacity - LargeBucket->Remainder;
}

void
ResetBuckets(bucket *LargeBucket, bucket *SmallBucket)
{
	LargeBucket->Water = SmallBucket->Water = 0;
}

void
Juggle(bucket *Giver, bucket *Taker)
{
	if (BucketIsEmpty(Giver))
	{
		FillBucket(Giver);
	}
	else if (BucketIsFull(Taker))
	{
		DumpBucket(Taker);
	}
	else
	{
		TransferBucket(Giver, Taker);
	}
}

bool32
Solve(bucket *LargeBucket, bucket *SmallBucket, uint32 Goal, bucket *StartBucket, int32 *StepsTaken)
{
	uint32 Remainder = (SmallBucket->Capacity) ? (Goal % SmallBucket->Capacity) : 0;

	if ((LargeBucket->Water == 0) && (SmallBucket->Water == 0))
	{
		FillBucket(StartBucket);
	}
	else if (Goal == LargeBucket->Capacity) // L
	{
		FillBucket(LargeBucket);
	}
	else if (Remainder == 0) // xS
	{
		Juggle(SmallBucket, LargeBucket);
	}
	else if (Remainder == LargeBucket->Remainder) // L - xS
	{
		Juggle(LargeBucket, SmallBucket);
	}
	else if (Remainder == SmallBucket->Remainder)
	{
		// SmallX is the amount of small bucket fills/transfers it takes to fill the large bucket
		int32 SmallX = (LargeBucket->Capacity + (SmallBucket->Capacity - 1)) / SmallBucket->Capacity;
		int32 PathACost = (2 * SmallX) + 4;
		int32 PathBCost = 4 * (SmallX - 1) + 2;

		if (PathACost < PathBCost)
			Juggle(SmallBucket, LargeBucket);
		else
			Juggle(LargeBucket, SmallBucket);
	}
	else
	{
		// NOTE(bret): It should never hit this. if it does, we have an edge case that we didn't prepare for
		printf("Infinite loop\n");
	}

	// TODO(bret): Find out if this is ever incorrect
	++(*StepsTaken);

	UpdateBucketsDisplay(LargeBucket, SmallBucket, *StepsTaken);

	if (BucketsHaveGoal(LargeBucket, SmallBucket, Goal))
	{
		return 1;
	}

	return 0;
}

// TODO(bret): Implement Steps I guess
int32
RunXSteps(bucket *LargeBucket, bucket *SmallBucket, uint32 Goal, bucket *StartBucket)
{
	ResetBuckets(LargeBucket, SmallBucket);

	// Reset console
	ClearConsole();
	printf("\tGoal: %d\n", Goal);

	int32 StepsTaken = 0;
	UpdateBucketsDisplay(LargeBucket, SmallBucket, StepsTaken);
	PauseFor(500);

	if (!BucketsHaveGoal(LargeBucket, SmallBucket, Goal))
	{
		while (1)
		{
			MoveCursorPosToStep(StepsTaken);
			printf("%d)\t", (StepsTaken + 1));
			if (Solve(LargeBucket, SmallBucket, Goal, StartBucket, &StepsTaken))
				break;
		}
	}

	MoveCursorPosToStep(StepsTaken);
	printf("Steps taken: %d\n", StepsTaken);

	WaitForInput();

	return StepsTaken;
}

bool32
IsSolvable(bucket *LargeBucket, bucket *SmallBucket, uint32 Goal)
{
	uint32 Remainder = (SmallBucket->Capacity) ? (Goal % SmallBucket->Capacity) : 0;

	char *ErrorStr = 0;

	if (Goal > LargeBucket->Capacity)
	{
		ErrorStr = "Goal is too large.\n";
	}
	else if (LargeBucket->Capacity == 0)
	{
		ErrorStr = "Large bucket must be greater than 0.\n";
	}
	else if (!((Goal == LargeBucket->Capacity) || (Remainder == 0) || (Remainder == LargeBucket->Remainder) || (Remainder == SmallBucket->Remainder)))
	{
		ErrorStr = "Unsolvable.\n";
	}
	else if ((SmallBucket->Capacity == 0) && (LargeBucket->Capacity != Goal))
	{
		ErrorStr = "Unsolvable.\n";
	}

	if (ErrorStr)
	{
		printf(ErrorStr);
		return 0;
	}

	return 1;
}

int main()
{
	bucket LargeBucket = { 0 };
	bucket SmallBucket = { 0 };
	StrCpy(LargeBucket.Name, "LARGE", ArrayCount(LargeBucket.Name));
	StrCpy(SmallBucket.Name, "SMALL", ArrayCount(SmallBucket.Name));

	SetColor(DEFAULT, BLACK);

	while (1)
	{
		int32 LargeCapacity = 0;
		int32 SmallCapacity = 0;
		int32 Goal = 0;
		PromptInt("Large bucket eize:\t", &LargeCapacity);
		PromptInt("Small bucket eize:\t", &SmallCapacity);
		PromptInt("Goal:\t\t\t", &Goal);

		InitBuckets(&LargeBucket, LargeCapacity, &SmallBucket, SmallCapacity);

		if (IsSolvable(&LargeBucket, &SmallBucket, Goal))
		{
			// Run on large and small buckets
			int32 LargeFirstSteps = RunXSteps(&LargeBucket, &SmallBucket, Goal, &LargeBucket);
			int32 SmallFirstSteps = RunXSteps(&LargeBucket, &SmallBucket, Goal, &SmallBucket);

			// Print results
			ClearConsole();
			
			printf("Steps if %s is filled first:\t%d\n", (char *)&LargeBucket, LargeFirstSteps);
			printf("Steps if %s is filled first:\t%d\n", (char *)&SmallBucket, SmallFirstSteps);

			if (LargeFirstSteps == SmallFirstSteps)
			{
				printf("Both solutions take the same amount of steps.\n");
			}
			else
			{
				bucket *LessBucket;
				if (LargeFirstSteps < SmallFirstSteps)
					LessBucket = &LargeBucket;
				else
					LessBucket = &SmallBucket;

				printf("Less steps are taken if the %s bucket is filled first.\n", (char *)LessBucket);
			}

			printf("\n");
		}

		WaitForInput();

		ClearConsole();

		// TODO(bret): Make it so it can break out
	}

	WaitForInput();
	return 0;
}
