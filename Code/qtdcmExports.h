#ifndef QTDCMEXPORT_H
#define QTDCMEXPORT_H

#ifdef WIN32
    #ifdef qtdcm_EXPORTS
        #define QTDCM_EXPORT __declspec(dllexport) 
    #else
        #define QTDCM_EXPORT __declspec(dllimport) 
    #endif
#else
    #define QTDCM_EXPORT
#endif

#endif

