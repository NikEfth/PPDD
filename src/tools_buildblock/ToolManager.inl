std::shared_ptr<PointerTool> ToolManager::mousePointer() const
{
    return mousePointer_sptr;
}

std::shared_ptr<CrossPointerTool> ToolManager::mouseCrossPointer() const
{
    return mouseCrossPointer_sptr;
}

std::shared_ptr<LineTool> ToolManager::mouseLinePointer() const
{
    return mouseLinePointer_sptr;
}

std::shared_ptr<RectTool> ToolManager::mouseRectPointer() const
{
    return mouseRectPointer_sptr;
}

std::shared_ptr<EclipseTool> ToolManager::mouseEllPointer() const
{
    return mouseEllPointer_sptr;
}

std::shared_ptr<Tool>  ToolManager::getCurrentTool() const
{
    return currentTool;
}

void ToolManager::unsetTool()
{
    if(sc != 0)
    {
        sc->my_containers[0]->my_screen->canvas()->removeEventFilter(currentTool.get());
        sc->my_containers[1]->my_screen->canvas()->removeEventFilter(currentTool.get());
        sc->my_containers[2]->my_screen->canvas()->removeEventFilter(currentTool.get());
        sc->refreshPlots();
    }
}


void ToolManager::setScreen(Screen_manager * _s)
{
    if(sc == 0)
    {
        sc = _s;
        _s->my_containers[0]->my_screen->canvas()->installEventFilter(currentTool.get());
        _s->my_containers[1]->my_screen->canvas()->installEventFilter(currentTool.get());
        _s->my_containers[2]->my_screen->canvas()->installEventFilter(currentTool.get());
    }
}

void ToolManager::unsetScreen()
{
    unsetTool();
    sc = 0;
}

void ToolManager::clearToolPalette()
{
    if (!stir::is_null_ptr(currentTool))
        currentTool->clearBeforeUnSet();
    unsetTool();
    p_cp->setHidden(true);
    p_lp->setHidden(true);
    p_rp->setHidden(true);
    p_ep->setHidden(true);
}
