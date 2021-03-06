#include "Managers/Lorries/JobObjectsLorry.h"

using namespace afermer;


struct HasHash
{
    int hash;
    explicit HasHash( int h ) : hash(h) { }
    bool operator() (JobObject::CPtr n) { return (n->id() == hash); }
};

struct HasName
{
    QString name;
    explicit HasName(const QString& n ) : name(n) { }
    bool operator() (JobObject::CPtr n) { return n->name() == name; }
};


JobObjectPtrIt JobObjectsLorry::find(int hash)
{
    return std::find_if( std::begin(m_objects), std::end(m_objects), HasHash(hash) );
}


JobObjectPtrIt JobObjectsLorry::find(const QString& name)
{
    return std::find_if( std::begin(m_objects), std::end(m_objects), HasName(name) );
}



JobObject::Ptr JobObjectsLorry::get(int hash)
{
    JobObject::Ptr ret;

    JobObjectPtrIt it = find(hash);
    if( it != m_objects.end() )
        ret = (*it);
    
    return ret;
}

void JobObjectsLorry::refresh()
{
    size_t len = m_objects.size();
    for(int i = 0; i < len; ++i)
    {
        JobObject::Ptr n = m_objects[i];
        if (!n->set_refreshed)
            m_deleted_jobs.push_back(n->m_id);

        n->set_refreshed = false;
    }

    for(int i = 0; i < m_deleted_jobs.size(); ++i)
        erase(m_deleted_jobs[i]);
    m_deleted_jobs.clear();
}



bool JobObjectsLorry::contain(int hash)
{
    JobObjectPtrIt it = find(hash);
    return (it != m_objects.end());
}

void JobObjectsLorry::erase(int hash)
{
    JobObjectPtrIt it = find(hash);
    if(it != m_objects.end())
        m_objects.erase(it);
}

void JobObjectsLorry::markToDelete(const QList<int>& i_indeces)
{
    for (size_t i = 0; i < i_indeces.size(); ++i)
        m_deleted_jobs.push_back( i_indeces[i] );
}

bool JobObjectsLorry::isDeleted(int hash)
{
    return ( std::find(m_deleted_jobs.begin(), m_deleted_jobs.end(), hash) != m_deleted_jobs.end() );
}

void JobObjectsLorry::clear()
{
    m_objects.clear();
    m_deleted_jobs.clear();
}


void JobObjectsLorry::insert(const QString& user_name
                  ,JobState::State status
                  ,const QString &time_creation
                  ,int blocks_num
                  ,const QString &working_time
                  ,const QString &hosts_mask
                  ,const QString &software
                  ,int priority
                  ,int slot
                  ,int progress
                  ,const QString &name
                  ,int id
                  ,int block_order
                  ,const QList<QString> &blades
                  ,const QString &block_name
                  ,int job_id
                  ,int blades_length
                  ,const QString &approx_time
                  ,const QString &depends
                  ,const QString &output_folder
                  ,const QString &user_color
                  ,int errors_avoid_blades
                  ,const std::string& json_represent)
{
    // if ( isDeleted(id) ) return;

    JobObjectPtrIt it = find(id);

    if( it != m_objects.end() )
    {
        (*it)->update(user_name
                ,status
                ,time_creation
                ,blocks_num
                ,working_time
                ,hosts_mask
                ,software
                ,priority
                ,slot
                ,progress
                ,name
                ,id
                ,block_order
                ,blades
                ,block_name
                ,job_id
                ,blades_length
                ,approx_time
                ,depends
                ,user_color
                ,errors_avoid_blades
                ,json_represent
            );
        (*it)->m_output_folder = output_folder;
        (*it)->set_refreshed = true;
    }
    else
    {
        JobObject::Ptr b = JobObject::create( user_name
                ,status
                ,time_creation
                ,blocks_num
                ,working_time
                ,hosts_mask
                ,software
                ,priority
                ,slot
                ,progress
                ,name
                ,id
                ,block_order
                ,blades
                ,block_name
                ,job_id
                ,blades_length
                ,approx_time
                ,depends
                ,user_color
                ,errors_avoid_blades
                ,json_represent
            );
        b->m_output_folder = output_folder;
        b->set_refreshed = true;
        m_objects.push_back(b);
    }
}



int JobObjectsLorry::lastTimeUpdatePercent(int hash, int percentage, int curtime)
{
    std::map<size_t, JobTimeApproximationManager::Ptr >::iterator m_job_time_it = m_job_time.find(hash);
    if (m_job_time_it == m_job_time.end())
    {
        m_job_time[hash] = JobTimeApproximationManager::create(percentage, curtime);
    }
    else
    {
        if (m_job_time[hash]->m_progress != percentage)
            m_job_time[hash] = JobTimeApproximationManager::create(percentage, curtime);
    }

    return m_job_time[hash]->m_time;
}