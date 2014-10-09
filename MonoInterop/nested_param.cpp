
#define Tmp(...) __VA_ARGS__
#define Stringnize1(...) #__VA_ARGS__
#define Stringnize2(...) Stringnize1(__VA_ARGS__)

#define Child(B) void Test::Tmp(Current)::##B() { printf(Stringnize2(Current) "::" #B); }

#define Current Hoge
	Child(Hage)
	Child(Hige)
#undef Current

void main()
{
}
