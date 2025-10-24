#include "../include/Poller.hpp"
#include "../include/Channel.hpp"
#include <cassert>
#include <cstring>
#include <cerrno>
#include <unistd.h>

Poller::Poller() {
    _epfd = epoll_create(MAX_EPOLLEVENTS);
    if(_epfd < 0) {
        L_ERROR << "epoll create failed";
        abort();
    }
}

void Poller::Update(Channel *channel, int op) {
    int fd = channel->Fd();
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = channel->Events();
    int ret = epoll_ctl(_epfd, op, fd, &ev);
    if(ret < 0) {
        L_ERROR << "epollCTL failed op=" << op << " fd=" << fd << " errno=" << errno << "(" << strerror(errno) << ")";
    }
}

bool Poller::HasChannel(Channel *channel)//问题出现在这里？---不是，但是这里也出错了-错误已经改了--是_channels和channel错乱的问题
{
    auto it=_channels.find(channel->Fd());
    if(it==_channels.end())
    {
        return false;
    }
    return true;
}
//添加或修改监控
void Poller::UpdateEvent(Channel *channel)
{
    bool ret = HasChannel(channel);
    if (ret == false) 
    {
        //不存在则添加
        _channels.insert(std::make_pair(channel->Fd(), channel));
        return Update(channel, EPOLL_CTL_ADD);
    }       
    return Update(channel, EPOLL_CTL_MOD);  // 仅首次添加时执行 ADD
}
//移除监控
void Poller::RemoveEvent(Channel *channel)
{
    auto it=_channels.find(channel->Fd());
    if(it!=_channels.end())
    {
        _channels.erase(it);
    }       
    Update(channel,EPOLL_CTL_DEL);
}
//开始监控，返回活跃链接
//void Poll(std::vector<Channel*>&active)///感觉问题在这里
void Poller::Poll(std::vector<Channel*>*active)
{
    //int epoll_wait(int epfd,struct epoll_even *evs,int maxevents,int timeout)
    int nfds=epoll_wait(_epfd,_evs,MAX_EPOLLEVENTS,-1);//-1为阻塞监控
    if(nfds<0)
    {
        //EINTR 阻塞被信号打断了
        if(errno/*---用的nfds,感觉有影响*/==EINTR)
        {
                L_DEBUG << "epoll_wait interrupted by signal";
            return;
        }
        L_ERROR << "Epollwait ERROR:" << strerror(errno);
        abort();
    }
    for(int i=0;i<nfds;i++)
    {
        auto it=_channels.find(_evs[i].data.fd);
        assert(it!=_channels.end());
        it->second->SetREvent(_evs[i].events);
        active->push_back(it->second);
        // if (it != _channels.end())
        // {
        //     Channel* channel = it->second;
        //     channel->SetREvent(_evs[i].events); // 设置返回事件
        //     active.push_back(channel);
        // }
        // else
        // {
        //     L_WARN("Epoll event for unknown fd=%d",_evs[i].data.fd); // 处理未知fd事件
        // }
    }

}