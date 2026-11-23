/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Ole-André Rodlie and contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "pathoperations.h"
#include "Boxes/pathbox.h"
#include "Animators/transformanimator.h"
#include "exceptions.h"
#include "pointhelpers.h"

// Closed contours of path only: an open contour (e.g. a dash) has no
// interior, filling it would close it with a straight chord.
static SkPath gClosedContours(const SkPath& path) {
    SkPath result;
    result.setFillType(path.getFillType());
    SkPath contour;
    SkPath::Iter iter(path, false);
    SkPoint pts[4];
    for(;;) {
        switch(iter.next(pts)) {
        case SkPath::kMove_Verb:
            contour.reset();
            contour.moveTo(pts[0]);
            break;
        case SkPath::kLine_Verb:
            contour.lineTo(pts[1]);
            break;
        case SkPath::kQuad_Verb:
            contour.quadTo(pts[1], pts[2]);
            break;
        case SkPath::kConic_Verb:
            contour.conicTo(pts[1], pts[2], iter.conicWeight());
            break;
        case SkPath::kCubic_Verb:
            contour.cubicTo(pts[1], pts[2], pts[3]);
            break;
        case SkPath::kClose_Verb:
            contour.close();
            result.addPath(contour);
            contour.reset();
            break;
        case SkPath::kDone_Verb:
            return result;
        }
    }
}

void gSolidify(const qreal widthT,
               const SkPath &src,
               SkPath * const dst) {
    if(isZero4Dec(widthT)) {
        *dst = src;
        return;
    }
    SkPathOp op = widthT < 0 ? SkPathOp::kDifference_SkPathOp :
                               SkPathOp::kUnion_SkPathOp;
    const qreal aWidth2 = qAbs(widthT*2);
    SkStroke strokerSk;
    strokerSk.setJoin(SkPaint::kRound_Join);
    strokerSk.setCap(SkPaint::kRound_Cap);
    strokerSk.setWidth(static_cast<float>(aWidth2));

    SkPath src2 = gPathToPolyline(src);
    SkPath outline;
    strokerSk.strokePath(src2, &outline);
    Op(src2, outline, op, dst);
    dst->setFillType(src.getFillType());

//    SkOpBuilder builder;
//    builder.add(src, SkPathOp::kUnion_SkPathOp);
//    const auto func = [&builder, &strokerSk, &op](const qCubicSegment2D& seg) {
//        qCubicSegment2D segC = seg;
//        SkPath lineSeg;
//        lineSeg.moveTo(toSkPoint(seg.p0()));
//        if(!seg.isLine()) {
//            for(qreal len = 10; len < segC.length(); len += 10) {
//                lineSeg.lineTo(toSkPoint(segC.posAtLength(len)));
//            }
//        }
//        lineSeg.lineTo(toSkPoint(seg.p3()));
//        SkPath outline;
//        strokerSk.strokePath(lineSeg, &outline);
//        builder.add(outline, op);
//    };
////    const auto func = [&builder, &strokerSk, &op](const SkPath& seg) {
////        SkPath outline;
////        strokerSk.strokePath(seg, &outline);
////        builder.add(outline, op);
////    };
//    gForEverySegmentInPath(src, func);

//    builder.resolve(dst);
}
