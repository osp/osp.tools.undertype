#ifndef __ULEXTRAGRAPHIC_INTERFACE_H
#define __ULEXTRAGRAPHIC_INTERFACE_H

class ULExtraGraphicInterface
{
public:
    ULExtraGraphicInterface() {}
    virtual ~ULExtraGraphicInterface() {}


private:
    ULExtraGraphicInterface( const ULExtraGraphicInterface& source );
    void operator = ( const ULExtraGraphicInterface& source );
};


#endif // __ULEXTRAGRAPHIC_INTERFACE_H
