#include "AVCLanDrv.h"
#include "AVCLanRx.h"
#include "AVCLanTx.h"
#include "AVCLanDrvRxTx.h"


AVCLanDrvBase::AVCLanDrvBase(p_timer timer)
	: timer(timer)
{
}
