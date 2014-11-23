/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef Icon_h
#define Icon_h

#include <wtf/Forward.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RetainPtr.h>
#include <wtf/Vector.h>

#if PLATFORM(IOS)
#include "NativeImagePtr.h"
#include <CoreGraphics/CoreGraphics.h>
#elif PLATFORM(MAC)
OBJC_CLASS NSImage;
#elif PLATFORM(WIN)
typedef struct HICON__* HICON;
#elif PLATFORM(GTK)
typedef struct _GdkPixbuf GdkPixbuf;
#elif PLATFORM(MUI)
#include "BALBase.h"
namespace WebCore {
class Image;
}
#endif

namespace WebCore {

class GraphicsContext;
class FloatRect;
    
class Icon : public RefCounted<Icon> {
public:
    WEBCORE_EXPORT static PassRefPtr<Icon> createIconForFiles(const Vector<String>& filenames);

    WEBCORE_EXPORT ~Icon();

    void paint(GraphicsContext&, const FloatRect&);

#if PLATFORM(WIN)
    static PassRefPtr<Icon> create(HICON hIcon) { return adoptRef(new Icon(hIcon)); }
#endif

#if PLATFORM(IOS)
    // FIXME: Make this work for non-iOS ports and remove the PLATFORM(IOS)-guard.
    WEBCORE_EXPORT static PassRefPtr<Icon> createIconForImage(NativeImagePtr);
#endif

private:
#if PLATFORM(IOS)
    Icon(CGImageRef);
    RetainPtr<CGImageRef> m_cgImage;
#elif PLATFORM(MAC)
    Icon(NSImage*);
    RetainPtr<NSImage> m_nsImage;
#elif PLATFORM(WIN)
    Icon(HICON);
    HICON m_hIcon;
#elif PLATFORM(GTK)
    Icon();
    GdkPixbuf* m_icon;
#elif PLATFORM(MUI)
    Icon();
    RefPtr<Image> m_icon;
#elif PLATFORM(EFL)
    Icon();
#endif
};

}

#endif