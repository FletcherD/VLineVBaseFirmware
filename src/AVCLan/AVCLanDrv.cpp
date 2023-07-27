#include "AVCLanDrv.h"
#include "AVCLanRx.h"
#include "AVCLanTx.h"


AVCLanDrvBase::AVCLanDrvBase(p_timer timer)
	: timer(timer)
{
}
