/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#ifndef LogicalSelectionOffsetCaches_h
#define LogicalSelectionOffsetCaches_h

#include "RenderBlock.h"

namespace WebCore {

class LogicalSelectionOffsetCaches {
public:
    class ContainingBlockInfo {
    public:
        ContainingBlockInfo()
            : m_block(0)
            , m_cache(0)
            , m_hasFloatsOrFlowThreads(false)
            , m_cachedLogicalLeftSelectionOffset(false)
            , m_cachedLogicalRightSelectionOffset(false)
        { }

        void setBlock(RenderBlock* block, const LogicalSelectionOffsetCaches* cache)
        {
            m_block = block;
            m_hasFloatsOrFlowThreads = m_hasFloatsOrFlowThreads || m_block->containsFloats() || m_block->flowThreadContainingBlock();
            m_cache = cache;
            m_cachedLogicalLeftSelectionOffset = false;
            m_cachedLogicalRightSelectionOffset = false;
        }

        RenderBlock* block() const { return m_block; }
        const LogicalSelectionOffsetCaches* cache() const { return m_cache; }

        LayoutUnit logicalLeftSelectionOffset(RenderBlock& rootBlock, LayoutUnit position) const
        {
            ASSERT(m_cache);
            if (m_hasFloatsOrFlowThreads || !m_cachedLogicalLeftSelectionOffset) {
                m_cachedLogicalLeftSelectionOffset = true;
                m_logicalLeftSelectionOffset = m_block->logicalLeftSelectionOffset(rootBlock, position, *m_cache);
            } else
                ASSERT(m_logicalLeftSelectionOffset == m_block->logicalLeftSelectionOffset(rootBlock, position, *m_cache));
            return m_logicalLeftSelectionOffset;
        }

        LayoutUnit logicalRightSelectionOffset(RenderBlock& rootBlock, LayoutUnit position) const
        {
            ASSERT(m_cache);
            if (m_hasFloatsOrFlowThreads || !m_cachedLogicalRightSelectionOffset) {
                m_cachedLogicalRightSelectionOffset = true;
                m_logicalRightSelectionOffset = m_block->logicalRightSelectionOffset(rootBlock, position, *m_cache);
            } else
                ASSERT(m_logicalRightSelectionOffset == m_block->logicalRightSelectionOffset(rootBlock, position, *m_cache));
            return m_logicalRightSelectionOffset;
        }

    private:
        RenderBlock* m_block;
        const LogicalSelectionOffsetCaches* m_cache;
        bool m_hasFloatsOrFlowThreads : 1;
        mutable bool m_cachedLogicalLeftSelectionOffset : 1;
        mutable bool m_cachedLogicalRightSelectionOffset : 1;
        mutable LayoutUnit m_logicalLeftSelectionOffset;
        mutable LayoutUnit m_logicalRightSelectionOffset;
        
    };

    explicit LogicalSelectionOffsetCaches(RenderBlock& rootBlock)
    {
#if ENABLE(TEXT_SELECTION)
        // FIXME: We should either move this assertion to the caller (if applicable) or structure the code
        // such that we can remove this assertion.
        ASSERT(rootBlock.isSelectionRoot());
#endif
        auto parent = rootBlock.parent();

        // LogicalSelectionOffsetCaches should not be used on an orphaned tree.
        m_containingBlockForFixedPosition.setBlock(parent->containingBlockForFixedPosition(), nullptr);
        m_containingBlockForAbsolutePosition.setBlock(parent->containingBlockForAbsolutePosition(), nullptr);
        m_containingBlockForInflowPosition.setBlock(parent->containingBlockForObjectInFlow(), nullptr);
    }

    LogicalSelectionOffsetCaches(RenderBlock& block, const LogicalSelectionOffsetCaches& cache)
        : m_containingBlockForFixedPosition(cache.m_containingBlockForFixedPosition)
        , m_containingBlockForAbsolutePosition(cache.m_containingBlockForAbsolutePosition)
    {
        if (block.canContainFixedPositionObjects())
            m_containingBlockForFixedPosition.setBlock(&block, &cache);

        if (block.canContainAbsolutelyPositionedObjects() && !block.isRenderInline() && !block.isAnonymousBlock())
            m_containingBlockForFixedPosition.setBlock(&block, &cache);

        m_containingBlockForInflowPosition.setBlock(&block, &cache);
    }

    const ContainingBlockInfo& containingBlockInfo(RenderBlock& block) const
    {
        EPosition position = block.style().position();
        if (position == FixedPosition) {
            ASSERT(block.containingBlock() == m_containingBlockForFixedPosition.block());
            return m_containingBlockForFixedPosition;
        }
        if (position == AbsolutePosition) {
            ASSERT(block.containingBlock() == m_containingBlockForAbsolutePosition.block());
            return m_containingBlockForAbsolutePosition;
        }
        ASSERT(block.containingBlock() == m_containingBlockForInflowPosition.block());
        return m_containingBlockForInflowPosition;
    }

private:
    ContainingBlockInfo m_containingBlockForFixedPosition;
    ContainingBlockInfo m_containingBlockForAbsolutePosition;
    ContainingBlockInfo m_containingBlockForInflowPosition;
};

} // namespace WebCore

#endif // LogicalSelectionOffsetCaches_h