
#include "Kernels/vnImageBicubic.h"

#include "Utilities/vnImageBlock.h"

VN_STATUS vnBicubicKernel( CONST CVImage & pSrcImage, FLOAT32 fCoeffB, FLOAT32 fCoeffC, FLOAT32 fX, FLOAT32 fY, UINT8 * pRawOutput )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !pRawOutput || !VN_IS_IMAGE_VALID(pSrcImage) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    FLOAT32 fSampleCount            = 0;
    VN_PIXEL_BLOCK gTotalBlock      = {0};
    VN_PIXEL_BLOCK gTempBlock       = {0};

    //
    // Scan the kernel space adding up the bicubic weights and pixel values
    //

    for ( INT32 j = -2 + 1; j <= 2; j++ )
    for ( INT32 i = -2 + 1; i <= 2; i++ )
    {
        INT32 iX = (INT32) fX + i;
        INT32 iY = (INT32) fY + j;

        if ( iX < 0 || iY < 0 || 
             iX > pSrcImage.QueryWidth() - 1 || 
             iY > pSrcImage.QueryHeight() - 1 )
        {
             continue;
        }

        UINT8 * pSrcPixel   = pSrcImage.QueryData() + pSrcImage.BlockOffset( iX, iY );

        FLOAT32 fXDelta     = (FLOAT32) fX - iX;
        FLOAT32 fYDelta     = (FLOAT32) fY - iY;
        FLOAT32 fDistance   = sqrtf( fXDelta * fXDelta + fYDelta * fYDelta );
        FLOAT32 fWeight     = vnBicubicWeight( fCoeffB, fCoeffC, (FLOAT32) fDistance );

        vnConvertToBlock( pSrcPixel, pSrcImage.QueryFormat(), &gTempBlock );

        if ( VN_IMAGE_PRECISION_FLOAT == gTempBlock.uiPrecision )
        {
            gTotalBlock.fChannelData[0] += fWeight * gTempBlock.fChannelData[0];
            gTotalBlock.fChannelData[1] += fWeight * gTempBlock.fChannelData[1];
            gTotalBlock.fChannelData[2] += fWeight * gTempBlock.fChannelData[2];
            gTotalBlock.fChannelData[3] += fWeight * gTempBlock.fChannelData[3];
        }
        else
        {
            gTotalBlock.iChannelData[0] += fWeight * gTempBlock.iChannelData[0];
            gTotalBlock.iChannelData[1] += fWeight * gTempBlock.iChannelData[1];
            gTotalBlock.iChannelData[2] += fWeight * gTempBlock.iChannelData[2];
            gTotalBlock.iChannelData[3] += fWeight * gTempBlock.iChannelData[3];
        }

        fSampleCount += fWeight;
    }

    //
    // Normalize our bicubic sum back to the valid pixel range
    //

    FLOAT32 fScaleFactor = 1.0f / fSampleCount;

    gTotalBlock.uiPrecision    = gTempBlock.uiPrecision;
    gTotalBlock.uiChannelCount = gTempBlock.uiChannelCount;

    if ( VN_IMAGE_PRECISION_FLOAT == gTempBlock.uiPrecision )
    {
        gTotalBlock.fChannelData[0] = fScaleFactor * gTotalBlock.fChannelData[0];
        gTotalBlock.fChannelData[1] = fScaleFactor * gTotalBlock.fChannelData[1];
        gTotalBlock.fChannelData[2] = fScaleFactor * gTotalBlock.fChannelData[2];
        gTotalBlock.fChannelData[3] = fScaleFactor * gTotalBlock.fChannelData[3];
    }
    else
    {
        gTotalBlock.iChannelData[0] = fScaleFactor * gTotalBlock.iChannelData[0];
        gTotalBlock.iChannelData[1] = fScaleFactor * gTotalBlock.iChannelData[1];
        gTotalBlock.iChannelData[2] = fScaleFactor * gTotalBlock.iChannelData[2];
        gTotalBlock.iChannelData[3] = fScaleFactor * gTotalBlock.iChannelData[3];
    }

    //
    // Write our weighted sum to our output
    //

    vnConvertFromBlock( gTotalBlock, pSrcImage.QueryFormat(), pRawOutput );

    return VN_SUCCESS;
}

VN_STATUS vnSampleBicubicVertical( CONST CVImage & pSrcImage, FLOAT32 fCoeffB, FLOAT32 fCoeffC, FLOAT32 fX, FLOAT32 fY, UINT8 * pRawOutput )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !pRawOutput || !VN_IS_IMAGE_VALID(pSrcImage) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    FLOAT32 fSampleCount            = 0;
    VN_PIXEL_BLOCK gTotalBlock      = {0};
    VN_PIXEL_BLOCK gTempBlock       = {0};

    //
    // Scan the kernel space adding up the bicubic weights and pixel values
    //

    for ( INT32 j = -2; j < 2; j++ )
    {
        INT32 iX = (INT32) fX;
        INT32 iY = (INT32) fY + j;

        if ( iX < 0 || iY < 0 || 
             iX > pSrcImage.QueryWidth() - 1 || 
             iY > pSrcImage.QueryHeight() - 1 )
        {
             continue;
        }

        UINT8 * pSrcPixel   = pSrcImage.QueryData() + pSrcImage.BlockOffset( iX, iY );

        FLOAT32 fYDelta     = (FLOAT32) fY - iY;
        FLOAT32 fDistance   = fabs( fYDelta );
        FLOAT32 fWeight     = vnBicubicWeight( fCoeffB, fCoeffC, (FLOAT32) fDistance );

        vnConvertToBlock( pSrcPixel, pSrcImage.QueryFormat(), &gTempBlock );

        if ( VN_IMAGE_PRECISION_FLOAT == gTempBlock.uiPrecision )
        {
            gTotalBlock.fChannelData[0] += fWeight * gTempBlock.fChannelData[0];
            gTotalBlock.fChannelData[1] += fWeight * gTempBlock.fChannelData[1];
            gTotalBlock.fChannelData[2] += fWeight * gTempBlock.fChannelData[2];
            gTotalBlock.fChannelData[3] += fWeight * gTempBlock.fChannelData[3];
        }
        else
        {
            gTotalBlock.iChannelData[0] += fWeight * gTempBlock.iChannelData[0];
            gTotalBlock.iChannelData[1] += fWeight * gTempBlock.iChannelData[1];
            gTotalBlock.iChannelData[2] += fWeight * gTempBlock.iChannelData[2];
            gTotalBlock.iChannelData[3] += fWeight * gTempBlock.iChannelData[3];
        }

        fSampleCount += fWeight;
    }

    //
    // Normalize our bicubic sum back to the valid pixel range
    //

    FLOAT32 fScaleFactor = 1.0f / fSampleCount;

    gTotalBlock.uiPrecision    = gTempBlock.uiPrecision;
    gTotalBlock.uiChannelCount = gTempBlock.uiChannelCount;

    if ( VN_IMAGE_PRECISION_FLOAT == gTempBlock.uiPrecision )
    {
        gTotalBlock.fChannelData[0] = fScaleFactor * gTotalBlock.fChannelData[0];
        gTotalBlock.fChannelData[1] = fScaleFactor * gTotalBlock.fChannelData[1];
        gTotalBlock.fChannelData[2] = fScaleFactor * gTotalBlock.fChannelData[2];
        gTotalBlock.fChannelData[3] = fScaleFactor * gTotalBlock.fChannelData[3];
    }
    else
    {
        gTotalBlock.iChannelData[0] = fScaleFactor * gTotalBlock.iChannelData[0];
        gTotalBlock.iChannelData[1] = fScaleFactor * gTotalBlock.iChannelData[1];
        gTotalBlock.iChannelData[2] = fScaleFactor * gTotalBlock.iChannelData[2];
        gTotalBlock.iChannelData[3] = fScaleFactor * gTotalBlock.iChannelData[3];
    }

    //
    // Write our weighted sum to our output
    //

    vnConvertFromBlock( gTotalBlock, pSrcImage.QueryFormat(), pRawOutput );

    return VN_SUCCESS;
}

VN_STATUS vnSampleBicubicHorizontal( CONST CVImage & pSrcImage, FLOAT32 fCoeffB, FLOAT32 fCoeffC, FLOAT32 fX, FLOAT32 fY, UINT8 * pRawOutput )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !pRawOutput || !VN_IS_IMAGE_VALID(pSrcImage) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    FLOAT32 fSampleCount            = 0;
    VN_PIXEL_BLOCK gTotalBlock      = {0};
    VN_PIXEL_BLOCK gTempBlock       = {0};

    //
    // Scan the kernel space adding up the bicubic weights and pixel values
    //

    for ( INT32 i = -2; i < 2; i++ )
    {
        INT32 iX = (INT32) fX + i;
        INT32 iY = (INT32) fY;

        if ( iX < 0 || iY < 0 || 
             iX > pSrcImage.QueryWidth() - 1 || 
             iY > pSrcImage.QueryHeight() - 1 )
        {
             continue;
        }

        UINT8 * pSrcPixel   = pSrcImage.QueryData() + pSrcImage.BlockOffset( iX, iY );

        FLOAT32 fXDelta     = (FLOAT32) fX - iX;
        FLOAT32 fDistance   = fabs( fXDelta );
        FLOAT32 fWeight     = vnBicubicWeight( fCoeffB, fCoeffC, (FLOAT32) fDistance );

        vnConvertToBlock( pSrcPixel, pSrcImage.QueryFormat(), &gTempBlock );

        if ( VN_IMAGE_PRECISION_FLOAT == gTempBlock.uiPrecision )
        {
            gTotalBlock.fChannelData[0] += fWeight * gTempBlock.fChannelData[0];
            gTotalBlock.fChannelData[1] += fWeight * gTempBlock.fChannelData[1];
            gTotalBlock.fChannelData[2] += fWeight * gTempBlock.fChannelData[2];
            gTotalBlock.fChannelData[3] += fWeight * gTempBlock.fChannelData[3];
        }
        else
        {
            gTotalBlock.iChannelData[0] += fWeight * gTempBlock.iChannelData[0];
            gTotalBlock.iChannelData[1] += fWeight * gTempBlock.iChannelData[1];
            gTotalBlock.iChannelData[2] += fWeight * gTempBlock.iChannelData[2];
            gTotalBlock.iChannelData[3] += fWeight * gTempBlock.iChannelData[3];
        }

        fSampleCount += fWeight;
    }

    //
    // Normalize our bicubic sum back to the valid pixel range
    //

    FLOAT32 fScaleFactor = 1.0f / fSampleCount;

    gTotalBlock.uiPrecision    = gTempBlock.uiPrecision;
    gTotalBlock.uiChannelCount = gTempBlock.uiChannelCount;

    if ( VN_IMAGE_PRECISION_FLOAT == gTempBlock.uiPrecision )
    {
        gTotalBlock.fChannelData[0] = fScaleFactor * gTotalBlock.fChannelData[0];
        gTotalBlock.fChannelData[1] = fScaleFactor * gTotalBlock.fChannelData[1];
        gTotalBlock.fChannelData[2] = fScaleFactor * gTotalBlock.fChannelData[2];
        gTotalBlock.fChannelData[3] = fScaleFactor * gTotalBlock.fChannelData[3];
    }
    else
    {
        gTotalBlock.iChannelData[0] = fScaleFactor * gTotalBlock.iChannelData[0];
        gTotalBlock.iChannelData[1] = fScaleFactor * gTotalBlock.iChannelData[1];
        gTotalBlock.iChannelData[2] = fScaleFactor * gTotalBlock.iChannelData[2];
        gTotalBlock.iChannelData[3] = fScaleFactor * gTotalBlock.iChannelData[3];
    }

    //
    // Write our weighted sum to our output
    //

    vnConvertFromBlock( gTotalBlock, pSrcImage.QueryFormat(), pRawOutput );

    return VN_SUCCESS;
}

VN_STATUS vnBicubicKernel( CONST CVImage & pSrcImage, FLOAT32 fCoeffB, FLOAT32 fCoeffC, FLOAT32 fX, FLOAT32 fY, BOOL bDirection, UINT8 * pRawOutput )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !pRawOutput || !VN_IS_IMAGE_VALID(pSrcImage) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    //
    // Compute the horizontal or vertical gaussian sample at the requested pixel coordinate.
    //
    
    switch ( bDirection )
    {
        case FALSE: return vnSampleBicubicHorizontal( pSrcImage, fCoeffB, fCoeffC, fX, fY, pRawOutput );
        case TRUE: return  vnSampleBicubicVertical( pSrcImage, fCoeffB, fCoeffC, fX, fY, pRawOutput );
    }

    return VN_ERROR_NOTIMPL;
}