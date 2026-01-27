#include "TestLevel.h"
#include "Actor/TestActor.h"

TestLevel::TestLevel()
{
	AddNewActor(new TestActor());
}
