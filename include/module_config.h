/*
 * AppSettings.h
 *
 *  Created on: 13 ��� 2015 �.
 *      Author: Anakod
 */

#include <SmingCore/SmingCore.h>

#ifndef INCLUDE_MODULE_CONFIG_H_
#define INCLUDE_MODULE_CONFIG_H_

void ModuleConfig_init();
void networkScanStart();
void networkScanCompleted(bool succeeded, BssList list);

#endif /* INCLUDE_APPSETTINGS_H_ */
