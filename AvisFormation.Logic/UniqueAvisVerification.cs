using AvisFormation.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AvisFormation.Logic
{
    public class UniqueAvisVerification
    {
        public bool EstAutoriseACommenter(string userId, int formationId)
        {
            using (var context = new AvisEntities())
            {
                var personEntity = context.Avis.FirstOrDefault(p => p.UserId == userId && p.IdFormation == formationId);
                if (personEntity == null)
                {
                    return true;
                }
                return false;
            }
        }

    }
}
