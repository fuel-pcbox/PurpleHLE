#include "ThreadPort.h"
#include "Thread.h"

bool ThreadPort::Process(IpcKernMsg* message, IpcKernMsg* reply)
{
	switch (message->ID())
	{

	case THREAD_TERMINATE:
		{
			OUTPUT(u32, 0); //still return some junk?
			ActiveThreads::Current()->Terminate(); //may throw
		}
		break;

	case THREAD_GET_STATE:
		{
			INPUT(u32, flavor);
			INPUT(u32, oldCount);

			u32* context = new u32[oldCount];
			u32 newCount = m_thread->GetContext(flavor, context, oldCount);

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, newCount);
			OUTPUT_ARRAY(u32, context, newCount);
		}
		break;

	case THREAD_SET_STATE:
		{
			INPUT(u32, flavor);
			INPUT(u32, newCount);
			INPUT_ARRAY(u32, context, newCount);

			m_thread->SetContext(flavor, context, newCount);

			OUTPUT(u32, 0); //retCode
		}
		break;

	case THREAD_RESUME:
		{
			m_thread->Resume();
			OUTPUT(u32, 0); //retCode
		}
		break;

	case THREAD_POLICY:			//don't care for now
	case THREAD_POLICY_SET:	
		{
			OUTPUT(u32, 0); 
		}
		break;

	default:
		Fail(message->ID());
	}
	return true;
}