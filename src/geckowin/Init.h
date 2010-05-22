//copyright xiphos; license gpl2


#ifndef __PCF_XULRUNNER_INIT_H__
#define __PCF_XULRUNNER_INIT_H__

bool init_xulrunner(const char* xul_dir, const char* app_dir);
//nsresult setup_user_agent(const char* vendor, const char* vendor_sub,
//        const char* comment);
void shutdown_xulrunner();
void addref(void* object);
void release(void* object);

#endif /* __PCF_XULRUNNER_INIT_H__ */
