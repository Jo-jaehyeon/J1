#include "J1ItemDefine.h"

void FJ1ItemStatContainer::AddStack(FName StatKey, int32 StackCount)
{
	if (StatKey.IsNone() || StackCount == 0) return;

	int32& Existing = StatMap.FindOrAdd(StatKey);
	Existing = FMath::Max(0, Existing + StackCount);

	if (Existing == 0)
	{
		StatMap.Remove(StatKey);
	}
}

void FJ1ItemStatContainer::RemoveStack(FName StatKey)
{
	StatMap.Remove(StatKey);
}

int32 FJ1ItemStatContainer::GetStackCount(FName StatKey) const
{
	const int32* Found = StatMap.Find(StatKey);
	return Found ? *Found : 0;
}

bool FJ1ItemStatContainer::ContainsKey(FName StatKey) const
{
	return StatMap.Contains(StatKey);
}